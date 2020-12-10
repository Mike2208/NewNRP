/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef SIMULATION_STATUS_H
#define SIMULATION_STATUS_H

#include "nrp_general_library/config/simulation_config.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

class SimulationStatus;

/*!
 * \brief Simulation status constants
 */
struct SimulationStatusConst
{
	/*!
	 * \brief Simulation status
	 */
	enum SIM_STATE
	{
		ERROR = -1,				// Simulation is in undefined state
		STARTUP,				// SimulationManager is waiting for config files
		CONFIG_LOADED,			// Config files loaded
		ENGINE_SETUP,			// Engines are being set up
		SIMULATION_LOADED,		// Simulation (Engines and TransceiverFunctions) is loaded, sim is ready to run
		RUNNING,				// Simulation is running
		PAUSED					// Simulation is paused
	};

	/*!
	 * \brief Simulation Status
	 */
	static constexpr FixedString Status = "SimStatus";

	/*!
	 * \brief Simulation Time
	 */
	static constexpr FixedString Time   = "Time";

	/*!
	 * \brief Simulation Config
	 */
	static constexpr FixedString Config = "SimConfig";

	using SPropNames = PropNames<Status, Time, Config>;

	using json_property_template_t = JSONConfigProperties<SimulationStatus, SPropNames, SIM_STATE, float, SimulationConfig>;
};

/*!
 * \brief Simulation status. Can be de/-serialized as JSON
 */
class SimulationStatus
        : public SimulationStatusConst,
          public SimulationStatusConst::json_property_template_t
{
	public:
		SimulationStatus(const nlohmann::json &json);
		SimulationStatus(SIM_STATE status, float time, SimulationConfig config);

		SIM_STATE status() const;
		SIM_STATE &status();

		float time() const;
		float &time();

		const SimulationConfig &config() const;
		SimulationConfig &config();
};

/*!
 * \brief Data sent to SimulationServer with SimulationServer::PostSimRunningCommand command
 */
struct SimulationRunningData
{
	static constexpr auto IgnoreTimeout = std::numeric_limits<float>::quiet_NaN();

	/*!
	 * \brief Should simulation run (true) or be paused (false)?
	 */
	bool SetRunning;

	/*!
	 * \brief Simulation timeout. If set to IgnoreTimeout or negative value, the simulation's timeout won't be changed
	 */
	float TimeOut = IgnoreTimeout;

	SimulationRunningData() = default;
	SimulationRunningData(bool _setRunning, float _timeOut);
};

#endif // SIMULATION_STATUS_H
