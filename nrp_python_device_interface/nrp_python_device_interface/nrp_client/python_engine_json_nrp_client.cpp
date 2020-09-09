#include "nrp_python_device_interface/nrp_client/python_engine_json_nrp_client.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include <chrono>

PythonEngineJSONNRPClient::PythonEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : EngineJSONNRPClient(config, std::move(launcher))
{}

PythonEngineJSONNRPClient::~PythonEngineJSONNRPClient()
{}

PythonEngineJSONNRPClient::RESULT PythonEngineJSONNRPClient::initialize()
{
	const auto &pythonConfig = this->engineConfig();
	nlohmann::json resp = this->sendInitCommand(nlohmann::json({{PythonConfig::ConfigType.m_data, pythonConfig->writeConfig()}}));
	if(!resp.at(PythonConfig::InitFileExecStatus.data()).get<bool>())
	{
		// Write the error message
		this->_initErrMsg = resp.at(PythonConfig::InitFileErrorMsg.data());
		std::cerr << this->_initErrMsg << std::endl;

		return PythonEngineJSONNRPClient::ERROR;
	}

	return PythonEngineJSONNRPClient::SUCCESS;
}

EngineInterface::RESULT PythonEngineJSONNRPClient::shutdown()
{
	this->sendShutdownCommand(nlohmann::json());

	return PythonEngineJSONNRPClient::SUCCESS;
}
