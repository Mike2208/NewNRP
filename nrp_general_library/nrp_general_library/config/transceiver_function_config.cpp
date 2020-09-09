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
