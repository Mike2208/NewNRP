#include "nrp_simulation/server/simulation_status.h"


SimulationStatus::SimulationStatus(const nlohmann::json &json)
    : json_property_template_t(json)
{}

SimulationStatus::SimulationStatus(SimulationStatusConst::SIM_STATE status, float time, SimulationConfig config)
    : json_property_template_t(status, time, config)
{}

SimulationStatusConst::SIM_STATE SimulationStatus::status() const
{
	return this->getPropertyByName<Status>();
}

SimulationStatusConst::SIM_STATE &SimulationStatus::status()
{
	return this->getPropertyByName<Status>();
}

float SimulationStatus::time() const
{
	return this->getPropertyByName<Time>();
}

float &SimulationStatus::time()
{
	return this->getPropertyByName<Time>();
}

const SimulationConfig &SimulationStatus::config() const
{
	return this->getPropertyByName<Config>();
}

SimulationConfig &SimulationStatus::config()
{
	return this->getPropertyByName<Config>();
}

SimulationRunningData::SimulationRunningData(bool _setRunning, float _timeOut)
    : SetRunning(_setRunning),
      TimeOut(_timeOut)
{}
