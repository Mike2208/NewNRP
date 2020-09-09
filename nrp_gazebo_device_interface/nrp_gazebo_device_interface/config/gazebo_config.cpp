#include "nrp_gazebo_device_interface/config/gazebo_config.h"

#include "nrp_gazebo_device_interface/config/nrp_gazebo_cmake_constants.h"
#include "nrp_general_library/config_headers/nrp_cmake_constants.h"

#include <iostream>

const std::vector<std::string> GazeboConfigConst::DefGazeboPlugins = {};

const size_t GazeboConfigConst::DefGazeboRNGSeed = static_cast<size_t>(time(nullptr));

GazeboConfig::GazeboConfig(EngineConfigConst::config_storage_t &config)
    : EngineJSONConfig(config,
                       GazeboConfig::DefGazeboPlugins, GazeboConfig::DefGazeboRNGSeed,
                       GazeboConfig::DefMaxWorldLoadTime)
{}

std::string &GazeboConfig::gazeboWorldFile()
{
	return this->template getPropertyByName<GazeboConfig::GazeboWorldFile, std::string>();
}

const std::string &GazeboConfig::gazeboWorldFile() const
{
	return this->template getPropertyByName<GazeboConfig::GazeboWorldFile, std::string>();
}

std::vector<std::string> &GazeboConfig::gazeboPlugins()
{
	return this->template getPropertyByName<GazeboConfig::GazeboPlugins, std::vector<std::string>>();
}

const std::vector<std::string> &GazeboConfig::gazeboPlugins() const
{
	return this->template getPropertyByName<GazeboConfig::GazeboPlugins, std::vector<std::string>>();
}

size_t &GazeboConfig::gazeboRNGSeed()
{
	return this->template getPropertyByName<GazeboConfig::GazeboRNGSeed, size_t>();
}

const size_t &GazeboConfig::gazeboRNGSeed() const
{
	return this->template getPropertyByName<GazeboConfig::GazeboRNGSeed, size_t>();
}

unsigned int &GazeboConfig::maxWorldLoadTime()
{
	return this->template getPropertyByName<GazeboConfig::MaxWorldLoadTime>();
}

const unsigned int &GazeboConfig::maxWorldLoadTime() const
{
	return this->template getPropertyByName<GazeboConfig::MaxWorldLoadTime>();
}

EngineConfigConst::string_vector_t GazeboConfig::allEngineProcEnvParams() const
{
	EngineConfigConst::string_vector_t envVars = this->EngineJSONConfig::allEngineProcEnvParams();

	// Add NRP and Gazebo plugins dir
	envVars.push_back("GAZEBO_PLUGIN_PATH=" NRP_GAZEBO_PLUGINS_DIR ":" DEFAULT_GAZEBO_PLUGIN_DIR ":$GAZEBO_PLUGIN_PATH");

	// Add NRP and Gazebo library paths
	envVars.push_back("LD_LIBRARY_PATH=" NRP_LIB_INSTALL_DIR ":" DEFAULT_GAZEBO_LIB_DIRS ":$LD_LIBRARY_PATH");

	// Add Gazebo models path
	envVars.push_back("GAZEBO_MODEL_PATH=" DEFAULT_GAZEBO_MODEL_DIR ":${GAZEBO_MODEL_PATH}");

	return envVars;
}

EngineConfigConst::string_vector_t GazeboConfig::allEngineProcStartParams() const
{
	EngineConfigConst::string_vector_t startParams = this->EngineJSONConfig::allEngineProcStartParams();

	// Add gazebo plugins
	for(const auto &curPlugin : this->gazeboPlugins())
	{
		startParams.push_back(GazeboConfig::GazeboPluginArg.data());
		startParams.push_back(curPlugin);
	}

	// Add gazebo communication system plugin
	startParams.push_back(GazeboConfig::GazeboPluginArg.data());
	startParams.push_back(NRP_GAZEBO_COMMUNICATION_PLUGIN);

	// Add RNG Seed
	startParams.push_back(GazeboConfig::GazeboRNGSeedArg.data());
	startParams.push_back(std::to_string(this->gazeboRNGSeed()));

	// Add world file
	startParams.push_back(this->gazeboWorldFile());

	return startParams;
}

bool GazeboConfig::checkEnvVariableFormat(const std::string &envVariable)
{
	const auto eqPos = envVariable.find_first_of('=');

	// Check that an '=' is present, and neither the first nor last element in the sting
	if(eqPos == 0 || eqPos >= envVariable.length()-1)
		return false;
	return true;
}
