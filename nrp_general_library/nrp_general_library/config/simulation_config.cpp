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

#include "nrp_general_library/config/simulation_config.h"

#include "nrp_general_library/config/config_formats/json_config_properties.h"

#include <iostream>

const SimulationConfig::tf_configs_t SimulationConfigConst::DefTFArrayConfig = {};
const SimulationConfig::tf_configs_t SimulationConfigConst::DefEngineSimulatorsConfig = {};

SimulationConfig::SimulationConfig(const nlohmann::json &configuration)
    : JSONConfigProperties(configuration,
                           DefSimulationTimeout,
                           DefEngineSimulatorsConfig,
						   DefTFArrayConfig)
{}

const std::string &SimulationConfig::name() const
{
	return this->getPropertyByName<SimulationConfig::SimName>();
}

std::string &SimulationConfig::name()
{
	return this->getPropertyByName<SimulationConfig::SimName>();
}

unsigned int SimulationConfig::simulationTimeOut() const
{
	return this->getPropertyByName<SimulationConfig::SimulationTimeout, unsigned int>();
}

unsigned int &SimulationConfig::simulationTimeOut()
{
	return this->getPropertyByName<SimulationConfig::SimulationTimeout, unsigned int>();
}

const SimulationConfig::tf_configs_t &SimulationConfig::engineConfigs() const
{
	return this->getPropertyByName<SimulationConfig::EngineSimulatorsConfig>();
}

SimulationConfig::tf_configs_t &SimulationConfig::engineConfigs()
{
	return this->getPropertyByName<SimulationConfig::EngineSimulatorsConfig>();
}

const SimulationConfig::tf_configs_t &SimulationConfig::transceiverFunctionConfigs() const
{
	return this->getPropertyByName<SimulationConfig::TFArrayConfig>();
}

SimulationConfig::tf_configs_t &SimulationConfig::transceiverFunctionConfigs()
{
	return this->getPropertyByName<SimulationConfig::TFArrayConfig>();
}

