//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_nest_server_engine/nrp_client/nest_engine_server_nrp_client.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/restclient_setup.h"
#include "nrp_nest_server_engine/config/cmake_constants.h"

#include <boost/tokenizer.hpp>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>


NestEngineServerNRPClient::NestEngineServerNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : Engine(config, std::move(launcher))
{
	RestClientSetup::ensureInstance();
}

NestEngineServerNRPClient::~NestEngineServerNRPClient()
{}

void NestEngineServerNRPClient::initialize()
{
	const auto servAddr = this->serverAddress();

	// Read initFile into string
	std::string initCode;
	{
		std::ifstream initFile(this->engineConfig()->nestInitFileName());
		initCode.assign((std::istreambuf_iterator<char>(initFile) ),
		                (std::istreambuf_iterator<char>()		) );
	}

	try
	{
		this->_parseFcn = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["__fcnParse"];
	}
	catch(const boost::python::error_already_set &)
	{
		NRPException::logCreate("Couldn't load param parse fcn: " + handle_pyerror());
	}

	auto timeout = SimulationTime::max();
	if(this->engineConfigGeneral()->engineCommandTimeout() > 0.f)
		timeout = toSimulationTime<float, std::ratio<1,1> >(this->engineConfigGeneral()->engineCommandTimeout());

	// Try sending data to given address. Continue until timeout
	const auto startTime = std::chrono::system_clock::now();
	bool initSuccess = false;
	do
	{
		// Check that process is still running
		if(this->_process->getProcessStatus() == ProcessLauncherInterface::ENGINE_RUNNING_STATUS::STOPPED)
			throw NRPException::logCreate("Nest Engine process stopped unexpectedly before the init file could be sent");

		// Try sending and executing initFile contents
		RestClient::Response resp;
		try
		{
			resp = RestClient::post(servAddr + "/exec", "application/json", nlohmann::json({{"source", initCode}}).dump());
		}
		catch(std::exception &)
		{
			// Server unreachable
			resp.code = 7;
		}

		// Retry if server still starting up
		if(resp.code == 7)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		// Check received status
		if(resp.code != 200)
			throw NRPException::logCreate("Failed to execute init file \"" + this->engineConfig()->nestInitFileName() + "\": " + resp.body);
		else
		{
			initSuccess = true;
			break;
		}
	}
	while(std::chrono::system_clock::now() - startTime > timeout);

	// Check if timeout reached
	if(!initSuccess)
		throw NRPException::logCreate("Failed to initialize Nest server. Received no response before timeout reached");

	// Run Prepare(). runLoopStep() can now use Run() for stepping
	const auto resp = RestClient::post(servAddr + "/api/Prepare", "text/plain", "");
	if(resp.code != 200)
		throw NRPException::logCreate("Failed to run nest.Prepare()");
}

void NestEngineServerNRPClient::shutdown()
{
	// Run Cleanup() to mirror Prepare() of initialize() call
	auto resp = RestClient::post(this->serverAddress() + "/api/Cleanup", "text/plain", "");
	if(resp.code != 200)
		throw NRPException::logCreate("Failed to run nest.Cleanup()");
}

SimulationTime NestEngineServerNRPClient::getEngineTime() const
{
	// Get Engine Time from Nest Kernel
	auto resp = RestClient::post(this->serverAddress() + "/api/GetKernelStatus", "application/json", "[\"time\"]");
	if(resp.code != 200)
		throw NRPException::logCreate("Failed to get Nest Kernel Status");

	return toSimulationTime<float, std::milli>(std::stof(resp.body));
}

void NestEngineServerNRPClient::runLoopStep(SimulationTime timeStep)
{
	this->_runStepThread = std::async(std::launch::async, &NestEngineServerNRPClient::runStepFcn, this, timeStep);
}

void NestEngineServerNRPClient::waitForStepCompletion(float timeOut)
{
	// If thread state is invalid, loop thread has completed and waitForStepCompletion was called once before
	if(!this->_runStepThread.valid())
		return;

	if(this->_runStepThread.wait_for(std::chrono::duration<double>(timeOut)) != std::future_status::ready)
		throw NRPException::logCreate("Nest loop still running after timeout reached");

	if(!this->_runStepThread.get())
		throw NRPException::logCreate("Nest loop failed unexpectedly");
}

EngineInterface::device_outputs_set_t NestEngineServerNRPClient::requestOutputDeviceCallback(const EngineInterface::device_identifiers_t &deviceIdentifiers)
{
	const auto serverAddr = this->serverAddress();

	EngineInterface::device_outputs_set_t retVals;
	//retVals.reserve(deviceIdentifiers.size());

	for(const auto &devID : deviceIdentifiers)
	{
		if(devID.EngineName == this->engineName())
		{
			const auto [call, params] = this->parseName(devID.Name);

			auto resp = RestClient::post(serverAddr + "/api/" + call, "application/json", params);
			if(resp.code != 200)
			    throw std::runtime_error("Failed to get data for device \"" + devID.Name + "\"");

			retVals.emplace(new NestServerDevice(DeviceIdentifier(devID), resp.body));
		}
	}

	return retVals;
}

void NestEngineServerNRPClient::handleInputDevices(const EngineInterface::device_inputs_t &inputDevices)
{
	const auto serverAddr = this->serverAddress();

	for(DeviceInterface *const inDev : inputDevices)
	{
		// If type cannot be processed, skip it
		if(inDev->id().Type != NestServerDevice::TypeName.m_data)
			continue;

		// Only process devices for this engine
		if(inDev->engineName() != this->engineName())
			continue;

		// Send command along with parameters to nest
		const auto [call, params] = this->parseName(inDev->name());

		auto resp = RestClient::post(serverAddr + "/api/" + call, "application/json", params);
		if(resp.code != 200)
		    throw std::runtime_error("Failed to get data for device \"" + inDev->name() + "\"");
	}
}

bool NestEngineServerNRPClient::runStepFcn(SimulationTime timestep)
{
	// According to the NEST API documentation, Run accepts time to simulate in milliseconds and floating-point format

	auto timestepFloatMs = fromSimulationTime<float, std::milli>(timestep);

	auto resp = RestClient::post(this->serverAddress() + "/api/Run", "application/json", "[" + std::to_string(timestepFloatMs) + "]");
	if(resp.code != 200)
		return false;

	return true;
}

std::string NestEngineServerNRPClient::serverAddress() const
{
	return this->engineConfig()->nestServerHost() + ":" + std::to_string(this->engineConfig()->nestServerPort());
}

std::tuple<std::string, std::string> NestEngineServerNRPClient::parseName(const std::string &devName) const
{
	// The Nest command to execute is stored in the DeviceIdentifier's name
	// in the format Command(arg1, arg2, ..., kwarg1=dat1, kwarg2=dat2, ...). Extract the command
	// and parameters, and serialize in JSON format for transmit
	const auto cmdPos = devName.find_first_of('(');
	if(devName.empty() || devName.back() != ')' || cmdPos >= devName.npos)
		throw NRPException::logCreate("Invalid device Name. Misconfigured parentheses in device \"" + devName + "\"");

	boost::python::str pyArgJson(this->_parseFcn(devName));
	return std::make_tuple(devName.substr(0, cmdPos), (std::string)boost::python::extract<std::string>(pyArgJson));
}
