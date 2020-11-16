#include "nrp_python_device_interface/config/python_config.h"

#include "nrp_general_library/config/cmake_constants.h"

PythonConfig::PythonConfig(EngineConfigConst::config_storage_t &config)
    : PythonConfig(config.Data)
{
	config.Config = this;
}

PythonConfig::PythonConfig(const nlohmann::json &data)
    : EngineJSONConfig(data,
                       PythonConfig::DefPythonFileName.data())
{}

std::string &PythonConfig::pythonFileName()
{
	return this->getPropertyByName<PythonConfig::PythonFileName, std::string>();
}

const std::string &PythonConfig::pythonFileName() const
{
	return this->getPropertyByName<PythonConfig::PythonFileName, std::string>();
}

EngineConfigConst::string_vector_t PythonConfig::allEngineProcStartParams() const
{
	EngineConfigConst::string_vector_t startParams = this->EngineJSONConfig::allEngineProcStartParams();

	// Add JSON Server address (will be used by plugin)
	startParams.push_back(std::string("--") + PythonConfig::EngineServerAddrArg.data() + "=" + this->engineServerAddress());

	return startParams;
}
