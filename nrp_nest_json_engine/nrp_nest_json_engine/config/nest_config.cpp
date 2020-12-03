#include "nrp_nest_json_engine/config/nest_config.h"

#include "nrp_general_library/config/cmake_constants.h"

const size_t NestConfigConst::DefNestRNGSeed = static_cast<size_t>(time(nullptr));

NestConfig::NestConfig(EngineConfigConst::config_storage_t &config)
    : NestConfig(config.Data)
{
	config.Config = this;
}

NestConfig::NestConfig(const nlohmann::json &data)
    : EngineJSONConfig(data,
                       NestConfig::DefNestRNGSeed, NestConfig::DefNestInitFileName.data())
{}

size_t NestConfig::nestRNGSeed() const
{
	return this->getPropertyByName<NestConfig::NestRNGSeed, size_t>();
}

size_t &NestConfig::nestRNGSeed()
{
	return this->getPropertyByName<NestConfig::NestRNGSeed, size_t>();
}

std::string &NestConfig::nestInitFileName()
{
	return this->getPropertyByName<NestConfig::NestInitFileName, std::string>();
}

const std::string &NestConfig::nestInitFileName() const
{
	return this->getPropertyByName<NestConfig::NestInitFileName, std::string>();
}

EngineConfigConst::string_vector_t NestConfig::allEngineProcEnvParams() const
{
	EngineConfigConst::string_vector_t envVars = this->EngineJSONConfig::allEngineProcEnvParams();;

	// Add NRP library path
	envVars.push_back("LD_LIBRARY_PATH=" NRP_LIB_INSTALL_DIR ":$LD_LIBRARY_PATH");

	// Disable Nest output
	envVars.push_back("PYNEST_QUIET=1");

	return envVars;
}

EngineConfigConst::string_vector_t NestConfig::allEngineProcStartParams() const
{
	EngineConfigConst::string_vector_t startParams = this->EngineJSONConfig::allEngineProcStartParams();

	// Add JSON Server address (will be used by plugin)
	startParams.push_back(std::string("--") + NestConfig::EngineServerAddrArg.data() + "=" + this->engineServerAddress());

	return startParams;
}
