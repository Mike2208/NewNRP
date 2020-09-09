#include "nrp_gazebo_device_interface/nrp_client/gazebo_engine_json_nrp_client.h"

#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>

GazeboEngineJSONNRPClient::GazeboEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : EngineJSONNRPClient(config, std::move(launcher))
{}

GazeboEngineJSONNRPClient::RESULT GazeboEngineJSONNRPClient::initialize()
{
	// Wait for Gazebo to load world
	auto confDat = this->engineConfig()->writeConfig();
	const nlohmann::json initRes = this->sendInitCommand(confDat);
	if(!initRes[0].get<bool>())
		return GazeboEngineJSONNRPClient::ERROR;

	return GazeboEngineJSONNRPClient::SUCCESS;
}

EngineInterface::RESULT GazeboEngineJSONNRPClient::shutdown()
{
	this->sendShutdownCommand(nlohmann::json());

	return GazeboEngineJSONNRPClient::SUCCESS;
}
