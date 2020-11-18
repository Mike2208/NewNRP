#include "nrp_nest_server_engine/nrp_client/nest_engine_server_nrp_client.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/restclient_setup.h"

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

NestEngineServerNRPClient::RESULT NestEngineServerNRPClient::initialize()
{
	const auto servAddr = this->serverAddress();

	std::string initCode;
	{
		std::ifstream initFile(this->engineConfig()->nestInitFileName());
		initCode.assign((std::istreambuf_iterator<char>(initFile) ),
		                (std::istreambuf_iterator<char>()		) );
	}

	auto resp = RestClient::post(servAddr + "/exec", "text/plain", initCode);
	if(resp.code != 200)
		return NestEngineServerNRPClient::ERROR;

	resp = RestClient::post(servAddr + "/api/Prepare", "text/plain", "");
	if(resp.code != 200)
		return NestEngineServerNRPClient::ERROR;

	return NestEngineServerNRPClient::SUCCESS;
}

EngineInterface::RESULT NestEngineServerNRPClient::shutdown()
{
	auto resp = RestClient::post(this->serverAddress() + "/api/Cleanup", "text/plain", "");
	if(resp.code != 200)
		return NestEngineServerNRPClient::ERROR;

	return NestEngineServerNRPClient::SUCCESS;
}

float NestEngineServerNRPClient::getEngineTime() const
{
	auto resp = RestClient::post(this->serverAddress() + "/api/GetKernelStatus", "application/json", "[\"time\"]");
	if(resp.code != 200)
		throw std::runtime_error("Unable to get NestServer's time");

	return std::stof(resp.body);
}

EngineInterface::step_result_t NestEngineServerNRPClient::runLoopStep(float timeStep)
{
	this->_runStepThread = std::async(std::launch::async, &NestEngineServerNRPClient::runStepFcn, this, timeStep);

	return EngineInterface::SUCCESS;
}

EngineInterface::RESULT NestEngineServerNRPClient::waitForStepCompletion(float timeOut)
{
	// If thread state is invalid, loop thread has completed and waitForStepCompletion was called once before
	if(!this->_runStepThread.valid())
		return EngineInterface::SUCCESS;

	if(this->_runStepThread.wait_for(std::chrono::duration<double>(timeOut)) != std::future_status::ready)
		return EngineInterface::ERROR;

	return this->_runStepThread.get();
}

EngineInterface::device_outputs_set_t NestEngineServerNRPClient::requestOutputDeviceCallback(const EngineInterface::device_identifiers_t &deviceIdentifiers)
{
	const auto serverAddr = this->serverAddress();

	EngineInterface::device_outputs_set_t retVals;
	//retVals.reserve(deviceIdentifiers.size());

	for(const auto &devID : deviceIdentifiers)
	{
		auto devDatIt = this->_nestDevs.find(devID.Name);
		if(devDatIt == this->_nestDevs.end())
		{
			devDatIt = this->_nestDevs.emplace_hint(devDatIt, new NestServerDevice(devID));
		}
		else
		{

			auto *pDevDat = devDatIt->get();
			auto req = nlohmann::json({{"args", pDevDat->args().serialize()}});
			req.update(pDevDat->kwargs().serialize());
			auto resp = RestClient::post(serverAddr + "/api/" + pDevDat->cmd(), "application/json", req.dump(0));
			if(resp.code != 200)
				throw std::runtime_error("Failed to get data for device \"" + devID.Name + "\"");

			pDevDat->data().SerializedData = resp.body;
			pDevDat->data().deserialize();
		}

		retVals.insert(*devDatIt);
	}

	return retVals;
}

EngineInterface::RESULT NestEngineServerNRPClient::handleInputDevices(const EngineInterface::device_inputs_t &inputDevices)
{
	for(DeviceInterface *const inDev : inputDevices)
	{
		// If type cannot be processed, skip it
		if(inDev->id().Type != NestServerDevice::TypeName.m_data)
			continue;

		// Send command along with parameters to nest
		auto &nestDev = dynamic_cast<NestServerDevice&>(*inDev);
		auto req = nlohmann::json({{"args", nestDev.args().serialize()}});
		req.update(nestDev.kwargs().serialize());
		auto resp = RestClient::post(this->serverAddress() + "/api/" + nestDev.cmd(), "application/json", req.dump(0));
		if(resp.code != 200)
			throw std::runtime_error("Failed to get data for device \"" + nestDev.id().Name + "\"");

		nestDev.data().SerializedData = resp.body;
		nestDev.data().deserialize();

		auto devIt = this->_nestDevs.find(inDev->id().Name);
		if(devIt != this->_nestDevs.end())
			devIt = this->_nestDevs.erase(devIt);

		this->_nestDevs.emplace_hint(devIt, new NestServerDevice(std::move(nestDev)));
	}

	return EngineInterface::SUCCESS;
}

EngineInterface::RESULT NestEngineServerNRPClient::runStepFcn(float timestep)
{
	auto resp = RestClient::post(this->serverAddress() + "/api/Run", "application/json", "[" + std::to_string(timestep) + "]");
	if(resp.code != 200)
		return EngineInterface::ERROR;

	return EngineInterface::SUCCESS;
}

std::string NestEngineServerNRPClient::serverAddress() const
{
	return this->engineConfig()->nestServerHost() + ":" + std::to_string(this->engineConfig()->nestServerPort());
}
