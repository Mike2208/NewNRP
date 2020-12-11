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

#include "nrp_simulation/config/server_config.h"

ServerConfig::ServerConfig(const nlohmann::json &config)
    : JSONConfigProperties(config,
                           DefProcessLauncherType.data(), DefServerAddress.data(),
                           DefServerWorkingDirectory, DefMaxNumExperiments,
                           DefServerTimestep)
{}

std::string &ServerConfig::processLauncherType()
{	return this->getPropertyByName<ProcessLauncherType>();	}

const std::string &ServerConfig::processLauncherType() const
{	return this->getPropertyByName<ProcessLauncherType>();	}

std::string &ServerConfig::serverAddress()
{	return this->getPropertyByName<ServerAddress>();	}

const std::string &ServerConfig::serverAddress() const
{	return this->getPropertyByName<ServerAddress>();	}

std::filesystem::path &ServerConfig::serverWorkingDirectory()
{	return this->getPropertyByName<ServerWorkingDirectory>();	}

const std::filesystem::path &ServerConfig::serverWorkingDirectory() const
{	return this->getPropertyByName<ServerWorkingDirectory>();	}

std::filesystem::path ServerConfig::serverExperimentDirectory() const
{
	return this->serverWorkingDirectory() / ServerExperimentDir;
}

int32_t &ServerConfig::maxNumExperiments()
{	return this->getPropertyByName<MaxNumExperiments>();	}

int32_t ServerConfig::maxNumExperiments() const
{	return this->getPropertyByName<MaxNumExperiments>();	}

float &ServerConfig::serverTimestep()
{	return this->getPropertyByName<ServerTimestep>();	}

float ServerConfig::serverTimestep() const
{	return this->getPropertyByName<ServerTimestep>();	}
