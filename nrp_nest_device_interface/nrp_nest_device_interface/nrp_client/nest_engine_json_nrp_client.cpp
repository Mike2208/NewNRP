#include "nrp_nest_device_interface/nrp_client/nest_engine_json_nrp_client.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include <chrono>

NestEngineJSONNRPClient::NestEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : EngineGrpcClient(config, std::move(launcher))
{}

NestEngineJSONNRPClient::~NestEngineJSONNRPClient()
{}

NestEngineJSONNRPClient::RESULT NestEngineJSONNRPClient::initialize()
{
	const auto &nestConfig = this->engineConfig();
	try
	{
		this->sendInitCommand(nlohmann::json({{NestConfig::ConfigType.m_data, nestConfig->writeConfig()}}));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return NestEngineJSONNRPClient::ERROR;
	}
	
	return NestEngineJSONNRPClient::SUCCESS;
}

EngineInterface::RESULT NestEngineJSONNRPClient::shutdown()
{
	this->sendShutdownCommand(nlohmann::json());

	return NestEngineJSONNRPClient::SUCCESS;
}
