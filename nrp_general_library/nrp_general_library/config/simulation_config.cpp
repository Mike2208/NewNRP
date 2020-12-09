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

