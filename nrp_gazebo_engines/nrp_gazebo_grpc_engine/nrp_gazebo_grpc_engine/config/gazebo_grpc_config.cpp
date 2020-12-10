//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_gazebo_grpc_engine/config/gazebo_grpc_config.h"

#include "nrp_gazebo_grpc_engine/config/cmake_constants.h"
#include "nrp_general_library/config/cmake_constants.h"

#include <iostream>

const std::vector<std::string> GazeboGrpcConfigConst::DefGazeboPlugins = {};

const size_t GazeboGrpcConfigConst::DefGazeboRNGSeed = static_cast<size_t>(time(nullptr));

GazeboGrpcConfig::GazeboGrpcConfig(EngineConfigConst::config_storage_t &config)
    : EngineGRPCConfig(config,
                       GazeboGrpcConfig::DefGazeboPlugins, GazeboGrpcConfig::DefGazeboRNGSeed,
                       GazeboGrpcConfig::DefMaxWorldLoadTime)
{}

std::string &GazeboGrpcConfig::gazeboWorldFile()
{
	return this->template getPropertyByName<GazeboGrpcConfig::GazeboWorldFile, std::string>();
}

const std::string &GazeboGrpcConfig::gazeboWorldFile() const
{
	return this->template getPropertyByName<GazeboGrpcConfig::GazeboWorldFile, std::string>();
}

std::vector<std::string> &GazeboGrpcConfig::gazeboPlugins()
{
	return this->template getPropertyByName<GazeboGrpcConfig::GazeboPlugins, std::vector<std::string>>();
}

const std::vector<std::string> &GazeboGrpcConfig::gazeboPlugins() const
{
	return this->template getPropertyByName<GazeboGrpcConfig::GazeboPlugins, std::vector<std::string>>();
}

size_t &GazeboGrpcConfig::gazeboRNGSeed()
{
	return this->template getPropertyByName<GazeboGrpcConfig::GazeboRNGSeed, size_t>();
}

const size_t &GazeboGrpcConfig::gazeboRNGSeed() const
{
	return this->template getPropertyByName<GazeboGrpcConfig::GazeboRNGSeed, size_t>();
}

unsigned int &GazeboGrpcConfig::maxWorldLoadTime()
{
	return this->template getPropertyByName<GazeboGrpcConfig::MaxWorldLoadTime>();
}

const unsigned int &GazeboGrpcConfig::maxWorldLoadTime() const
{
	return this->template getPropertyByName<GazeboGrpcConfig::MaxWorldLoadTime>();
}

EngineConfigConst::string_vector_t GazeboGrpcConfig::allEngineProcEnvParams() const
{
	EngineConfigConst::string_vector_t envVars = this->EngineGRPCConfig::allEngineProcEnvParams();

	// Add NRP and Gazebo plugins dir
	envVars.push_back("GAZEBO_PLUGIN_PATH=" NRP_GAZEBO_PLUGINS_DIR ":" DEFAULT_GAZEBO_PLUGIN_DIR ":$GAZEBO_PLUGIN_PATH");

	// Add NRP and Gazebo library paths
	envVars.push_back("LD_LIBRARY_PATH=" NRP_LIB_INSTALL_DIR ":" DEFAULT_GAZEBO_LIB_DIRS ":" NRP_GAZEBO_PLUGINS_DIR ":" DEFAULT_GAZEBO_PLUGIN_DIR ":$LD_LIBRARY_PATH");

	// Add Gazebo models path
	envVars.push_back("GAZEBO_MODEL_PATH=" DEFAULT_GAZEBO_MODEL_DIR ":${GAZEBO_MODEL_PATH}");

	return envVars;
}

EngineConfigConst::string_vector_t GazeboGrpcConfig::allEngineProcStartParams() const
{
	EngineConfigConst::string_vector_t startParams = this->EngineGRPCConfig::allEngineProcStartParams();

	// Add gazebo plugins
	for(const auto &curPlugin : this->gazeboPlugins())
	{
		startParams.push_back(GazeboGrpcConfig::GazeboPluginArg.data());
		startParams.push_back(curPlugin);
	}

	// Add gazebo communication system plugin
	startParams.push_back(GazeboGrpcConfig::GazeboPluginArg.data());
	startParams.push_back(NRP_GAZEBO_COMMUNICATION_PLUGIN);

	startParams.push_back("--verbose");

	// Add RNG Seed
	startParams.push_back(GazeboGrpcConfig::GazeboRNGSeedArg.data());
	startParams.push_back(std::to_string(this->gazeboRNGSeed()));

	// Add world file
	startParams.push_back(this->gazeboWorldFile());

	return startParams;
}

bool GazeboGrpcConfig::checkEnvVariableFormat(const std::string &envVariable)
{
	const auto eqPos = envVariable.find_first_of('=');

	// Check that an '=' is present, and neither the first nor last element in the sting
	if(eqPos == 0 || eqPos >= envVariable.length()-1)
		return false;
	return true;
}
