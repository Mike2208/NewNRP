#include "nrp_gazebo_json_engine/nrp_client/gazebo_engine_json_nrp_client.h"

#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>

GazeboEngineJSONNRPClient::GazeboEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : EngineJSONNRPClient(config, std::move(launcher))
{}

void GazeboEngineJSONNRPClient::initialize()
{
	try
	{
		// Wait for Gazebo to load world
		auto confDat = this->engineConfig()->writeConfig();
		const nlohmann::json initRes = this->sendInitCommand(confDat);
		if(!initRes[0].get<bool>())
			throw NRPExceptionNonRecoverable("Received initialization fail message from Engine \"" + this->engineName() + "\"");
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Engine \"" + this->engineName() + "\" initialization failed");
	}
}

void GazeboEngineJSONNRPClient::shutdown()
{
	try
	{
		this->sendShutdownCommand(nlohmann::json());
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Engine \"" + this->engineName() + "\" shutdown failed");
	}
}
