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

#include "nrp_nest_json_engine/engine_server/nest_json_server.h"

#include "nrp_general_library/utils/python_error_handler.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_nest_json_engine/config/cmake_constants.h"
#include "nrp_nest_json_engine/engine_server/nest_engine_device_controller.h"
#include "nrp_nest_json_engine/python/create_device_class.h"

#include <fstream>

namespace python = boost::python;


NestJSONServer::NestJSONServer(const std::string &serverAddress, python::dict globals, python::object locals)
    : EngineJSONServer(serverAddress),
      _pyGlobals(globals),
      _pyLocals(locals)
{}

NestJSONServer::NestJSONServer(const std::string &serverAddress, const std::string &engineName, const std::string &registrationAddress, python::dict globals, boost::python::object locals)
    : EngineJSONServer(serverAddress, engineName, registrationAddress),
      _pyGlobals(globals),
      _pyLocals(locals)
{}

NestJSONServer::~NestJSONServer()
{
	try
	{
		// If Nest has been initialized, run cleanup
		if(this->_nestPreparedFlag && this->_pyNest.has_key("Cleanup"))
		{
			this->_nestPreparedFlag = false;
			this->_pyNest["Cleanup"]();
		}

		// Shutdown any running threads
		this->_shutdownFlag = true;
		this->shutdownServer();
	}
	catch(python::error_already_set &)
	{
		// If an error occured, print the error
		NRPException::logCreate("Nest JSON python shutdown failure: " + handle_pyerror());
	}
	catch(std::exception &e)
	{
		NRPException::logCreate(e, "Nest JSON python failure");
	}
}

bool NestJSONServer::initRunFlag() const
{
	return this->_initRunFlag;
}

bool NestJSONServer::shutdownFlag() const
{
	return this->_shutdownFlag;
}

SimulationTime NestJSONServer::runLoopStep(SimulationTime timeStep)
{
	PythonGILLock lock(this->_pyGILState, true);

	try
	{
		// Convert SimulationTime to milliseconds
		// NEST uses floating points for time variables, we have to convert our time step to a double

		const double timeStepMsDouble = fromSimulationTime<float, std::milli>(timeStep);

		// NEST Resolution is in milliseconds

		const auto resolutionMs = python::extract<double>(this->_pyNest["GetKernelStatus"]("resolution"));

		// Round the time step to account for NEST resolution

		const auto numSteps = std::round(timeStepMsDouble / resolutionMs);

		if(numSteps == 0)
		{
			throw NRPException::logCreate("Nest time step too small, step (ms): " + std::to_string(timeStepMsDouble) + ", resolution (ms): " + std::to_string(resolutionMs));
		}

		const double runTimeMsRounded = std::round(timeStepMsDouble / resolutionMs) * resolutionMs;

		this->_pyNest["Run"](runTimeMsRounded);

		// The time field of dictionary returned from GetKernelStatus contains time in milliseconds
		
		return toSimulationTime<float, std::milli>(python::extract<float>(this->_pyNest["GetKernelStatus"]("time")));
	}
	catch(python::error_already_set &)
	{
		// If an error occured, print the error
		throw NRPException::logCreate("Failed to run Nest step: " + handle_pyerror());
	}
}

nlohmann::json NestJSONServer::initialize(const nlohmann::json &data, EngineJSONServer::lock_t&)
{
	PythonGILLock lock(this->_pyGILState, true);
	try
	{
		// Import modules
		python::object nestModule = python::import("nest");
		this->_pyNest = python::dict(nestModule.attr("__dict__"));

		python::object nrpNestModule = python::import(NRP_NEST_PYTHON_MODULE_STR);
		this->_pyNRPNest = python::dict(nrpNestModule.attr("__dict__"));

		this->_pyGlobals["nest"] = nestModule;
		this->_pyGlobals[NRP_NEST_PYTHON_MODULE_STR] = nrpNestModule;

		// Set Nest to silent
		nestModule.attr("set_verbosity")(0);
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server without setting the initRunFlag
		const auto msg = handle_pyerror();
		NRPLogger::SPDErrLogDefault(msg);
		return this->formatInitErrorMessage(msg);
	}

	// Read received configuration
	const NestConfig config(data.at(NestConfig::ConfigType.m_data));

	// Empty device mapping
	this->_devMap.clear();

	// Read init file if present
	const auto &initFileName = config.nestInitFileName();
	if(!initFileName.empty())
	{
		std::fstream initFile(initFileName, std::ios_base::in);
		if(!initFile.good())
			return this->formatInitErrorMessage("Could not find init file " + initFileName);

		// Execute Init File
		try
		{
			python::exec_file(python::str(initFileName), this->_pyGlobals, this->_pyLocals);
		}
		catch(python::error_already_set &)
		{
			// If an error occured, return the message to the NRP server without setting the initRunFlag
			const auto msg = handle_pyerror();
			NRPLogger::SPDErrLogDefault(msg);
			return this->formatInitErrorMessage(msg);
		}

		initFile.close();
	}

	nlohmann::json jsonDevMap;
	try
	{
		// Read device map
		python::dict jsonModule = static_cast<python::dict>(python::import("json").attr("__dict__"));
		python::object jsonSerialize = jsonModule["dumps"];

		const std::string jsonStr = python::extract<std::string>(jsonSerialize(this->_devMap));
		jsonDevMap = nlohmann::json::parse(jsonStr);

		// Register devices
		this->_devMap = python::dict(this->_pyNRPNest["GetDevMap"]());
		python::list devMapKeys = this->_devMap.keys();
		const long numDevices = python::len(devMapKeys);
		//const long numDevices = python::len(this->_pyNRPNest["GetDevMap"]());
		for(long i=0; i < numDevices; ++i)
		{
			const python::object &devKey = devMapKeys[i];
			const std::string devName = python::extract<std::string>(python::str(devKey));
			python::object devNodes = this->_devMap[devKey];

			auto devController = std::shared_ptr<NestEngineJSONDeviceController<NestDevice> >(new
			            NestEngineJSONDeviceController<NestDevice>(DeviceIdentifier(devName, config.engineName(), NestDevice::TypeName.data()),
												 devNodes, this->_pyNest));

			this->_deviceControllerPtrs.push_back(devController);
			this->registerDeviceNoLock(devName, devController.get());
		}

		// Prepare Nest for execution
		this->_pyNest["Prepare"]();
		this->_nestPreparedFlag = true;
	}
	catch(python::error_already_set &)
	{
		// If an error occured, print the error
		const auto msg = handle_pyerror();
		NRPLogger::SPDErrLogDefault(msg);
		return this->formatInitErrorMessage(msg);
	}

	// Init has run once
	this->_initRunFlag = true;

	// Return success and parsed devmap
	return nlohmann::json({{NestConfig::InitFileExecStatus, true}, {NestConfig::InitFileParseDevMap, jsonDevMap}});
}

nlohmann::json NestJSONServer::shutdown(const nlohmann::json &)
{
	PythonGILLock lock(this->_pyGILState, true);

	this->_shutdownFlag = true;

	if(this->_nestPreparedFlag)
	{
		this->_nestPreparedFlag = false;
		this->_pyNest["Cleanup"]();
	}

	// Remove device controllers
	this->clearRegisteredDevices();
	this->_deviceControllerPtrs.clear();

	return nlohmann::json();
}

nlohmann::json NestJSONServer::formatInitErrorMessage(const std::string &errMsg)
{
	return nlohmann::json({{NestConfig::InitFileExecStatus, 0}, {NestConfig::InitFileErrorMsg, errMsg}});
}

nlohmann::json NestJSONServer::getDeviceData(const nlohmann::json &reqData)
{
	PythonGILLock lock(this->_pyGILState, true);
	return this->EngineJSONServer::getDeviceData(reqData);
}

nlohmann::json NestJSONServer::setDeviceData(const nlohmann::json &reqData)
{
	PythonGILLock lock(this->_pyGILState, true);
	return this->EngineJSONServer::setDeviceData(reqData);
}
