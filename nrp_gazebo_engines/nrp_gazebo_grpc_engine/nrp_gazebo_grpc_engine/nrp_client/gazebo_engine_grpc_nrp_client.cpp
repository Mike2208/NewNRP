#include "nrp_gazebo_grpc_engine/nrp_client/gazebo_engine_grpc_nrp_client.h"

#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>

GazeboEngineGrpcNRPClient::GazeboEngineGrpcNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : EngineGrpcClient(config, std::move(launcher))
{}

GazeboEngineGrpcNRPClient::RESULT GazeboEngineGrpcNRPClient::initialize()
{
	// Wait for Gazebo to load world
	auto confDat = this->engineConfig()->writeConfig();
	try
	{
		this->sendInitCommand(confDat);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return GazeboEngineGrpcNRPClient::ERROR;
	}

	return GazeboEngineGrpcNRPClient::SUCCESS;
}

EngineInterface::RESULT GazeboEngineGrpcNRPClient::shutdown()
{
	this->sendShutdownCommand(nlohmann::json());

	return GazeboEngineGrpcNRPClient::SUCCESS;
}
