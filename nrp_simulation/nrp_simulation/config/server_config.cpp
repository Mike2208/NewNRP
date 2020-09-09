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
