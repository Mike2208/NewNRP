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

	// Disable Nest output
	startParams.push_back("--quiet");

	return startParams;
}
