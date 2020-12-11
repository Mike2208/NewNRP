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

#include "nrp_general_library/config/transceiver_function_config.h"

TransceiverFunctionConfig::TransceiverFunctionConfig()
    : TransceiverFunctionConfig(nlohmann::json())
{}

TransceiverFunctionConfig::TransceiverFunctionConfig(SimulationConfig::config_storage_t &config)
    : TransceiverFunctionConfig(config.Data)
{
	// Register callback to allow updating SimulationConfig on TransceiverFunctionConfig change
	config.Config = this;
}

TransceiverFunctionConfig::TransceiverFunctionConfig(const nlohmann::json &config)
    : JSONConfigProperties(config,
                           DefName.data(), DefFile.data(), DefIsActive)
{}

const std::string &TransceiverFunctionConfig::fileName() const
{
	return this->getPropertyByName<TransceiverFunctionConfig::File, std::string>();
}

void TransceiverFunctionConfig::setFileName(const std::string &fileName)
{
	this->getPropertyByName<TransceiverFunctionConfig::File, std::string>() = fileName;
}

const std::string &TransceiverFunctionConfig::name() const
{
	return this->getPropertyByName<TransceiverFunctionConfigConst::Name, std::string>();
}

void TransceiverFunctionConfig::setName(const std::string &name)
{
	this->getPropertyByName<TransceiverFunctionConfigConst::Name, std::string>() = name;
}

bool TransceiverFunctionConfig::isActive() const
{
	return this->getPropertyByName<TransceiverFunctionConfig::IsActive, bool>();
}

void TransceiverFunctionConfig::setIsActive(bool active)
{
	this->getPropertyByName<TransceiverFunctionConfig::IsActive, bool>() = active;
}
