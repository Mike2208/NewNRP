#include "nrp_gazebo_grpc_engine/nrp_client/gazebo_engine_grpc_nrp_client.h"

#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>

GazeboEngineGrpcNRPClient::GazeboEngineGrpcNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : EngineGrpcClient(config, std::move(launcher))
{}

void GazeboEngineGrpcNRPClient::initialize()
{
	// Wait for Gazebo to load world
	auto confDat = this->engineConfig()->writeConfig();
	try
	{
		this->sendInitCommand(confDat);
	}
	catch(std::exception& e)
	{
		throw NRPException::logCreate(e, "Engine \"" + this->engineName() + "\" initialization failed");
	}
}

void GazeboEngineGrpcNRPClient::shutdown()
{
	try
	{
		this->sendShutdownCommand(nlohmann::json());
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate("Engine \"" + this->engineName() + "\" shutdown failed");
	}
}
