#include "nrp_nest_server_engine/nrp_client/nest_engine_server_nrp_client.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/restclient_setup.h"

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

	std::string initCode;
	{
		std::ifstream initFile(this->engineConfig()->nestInitFileName());
		initCode.assign((std::istreambuf_iterator<char>(initFile) ),
		                (std::istreambuf_iterator<char>()		) );
	}

	try
	{
		auto resp = RestClient::post(servAddr + "/exec", "text/plain", initCode);
		if(resp.code != 200)
			throw NRPException::logCreate("Failed to execute init file \"" + this->engineConfig()->nestInitFileName() + "\"");

		resp = RestClient::post(servAddr + "/api/Prepare", "text/plain", "");
		if(resp.code != 200)
			throw NRPException::logCreate("Failed to run nest.Prepare()");
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Nest Server Engine \"" + this->engineName() + "\" initialization failed");
	}
}

void NestEngineServerNRPClient::shutdown()
{
	auto resp = RestClient::post(this->serverAddress() + "/api/Cleanup", "text/plain", "");
	if(resp.code != 200)
		throw NRPException::logCreate("Failed to run nest.Cleanup()");
}

float NestEngineServerNRPClient::getEngineTime() const
{
	auto resp = RestClient::post(this->serverAddress() + "/api/GetKernelStatus", "application/json", "[\"time\"]");
	if(resp.code != 200)
		throw NRPException::logCreate("Failed to get Nest Kernel Status");

	return std::stof(resp.body);
}

void NestEngineServerNRPClient::runLoopStep(float timeStep)
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

std::tuple<std::string, std::string> extractDeviceRequest(const std::string &devName)
{
	const auto cmdPos = devName.find_first_of('(');
	if(devName.empty() || devName.back() != ')' || cmdPos >= devName.npos)
		throw NRPException::logCreate("Invalid device Name. Misconfigured parentheses in device \"" + devName + "\"");

	// Separate parameters by comma
	boost::tokenizer<boost::escaped_list_separator<char> >params(devName.substr(cmdPos+1, devName.size()-cmdPos-2),
	                                                             boost::escaped_list_separator<char>("\\", ",", "'\""));

	// Get all parameters
	nlohmann::json callParams = {{ "args", {}}};
	for(const auto &p : params)
	{
		//Check if current param is arg or kwarg
		const auto eqPos = p.find_first_of('=');
		if(eqPos == p.npos)
			callParams.push_back({p.substr(0, eqPos), p.substr(eqPos+1, p.size()-eqPos-1)});
		else
		{
			callParams[0].push_back(p);
		}
	}

	return std::make_tuple(devName.substr(0, cmdPos), callParams.dump());
};

EngineInterface::device_outputs_set_t NestEngineServerNRPClient::requestOutputDeviceCallback(const EngineInterface::device_identifiers_t &deviceIdentifiers)
{
	const auto serverAddr = this->serverAddress();

	EngineInterface::device_outputs_set_t retVals;
	//retVals.reserve(deviceIdentifiers.size());

	for(const auto &devID : deviceIdentifiers)
	{
		if(devID.EngineName == this->engineName())
		{
			auto [ call, params ] = extractDeviceRequest(devID.Name);

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
		auto [ call, params ] = extractDeviceRequest(inDev->name());

		auto resp = RestClient::post(serverAddr + "/api/" + call, "application/json", params);
		if(resp.code != 200)
		    throw std::runtime_error("Failed to get data for device \"" + inDev->name() + "\"");
	}
}

bool NestEngineServerNRPClient::runStepFcn(float timestep)
{
	auto resp = RestClient::post(this->serverAddress() + "/api/Run", "application/json", "[" + std::to_string(timestep) + "]");
	if(resp.code != 200)
		return false;

	return true;
}

std::string NestEngineServerNRPClient::serverAddress() const
{
	return this->engineConfig()->nestServerHost() + ":" + std::to_string(this->engineConfig()->nestServerPort());
}
