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

#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H

#include "nrp_general_library/config/base_config.h"
#include "nrp_general_library/config/config_formats/json_config_properties.h"
#include "nrp_general_library/config/config_storage.h"

#include <filesystem>

class SimulationConfig;

struct SimulationConfigConst
{
	using config_storage_t = ConfigStorage<nlohmann::json>;
	using tf_configs_t = std::vector<config_storage_t>;

	/*!
	 * \brief SimulationConfig Type
	 */
	static constexpr FixedString ConfigType = "SimulationConfig";

	/*!
	 * \brief Experiment Timeout (in seconds). 0 means no timeout
	 */
	static constexpr FixedString SimulationTimeout = "SimulationTimeout";
	static constexpr unsigned int DefSimulationTimeout = 0;

	/*!
	 * \brief Simulator configuration. Can vary depending on the type of brain
	 */
	static constexpr FixedString EngineSimulatorsConfig = "EngineConfigs";
	static const tf_configs_t DefEngineSimulatorsConfig;

	/*!
	 * \brief Simulator configuration. Can vary depending on the type of physics engine
	 */
	static constexpr FixedString TFArrayConfig = "TransceiverFunctions";
	static const tf_configs_t DefTFArrayConfig;

	/*!
	 * \brief Name of simulation
	 */
	static constexpr FixedString SimName = "SimulationName";

	using SPropNames = PropNames<SimulationConfigConst::SimulationTimeout,
	                             SimulationConfigConst::EngineSimulatorsConfig,
								 SimulationConfigConst::TFArrayConfig,
	                             SimName>;

	using SProps = JSONConfigProperties<SimulationConfig, SimulationConfigConst::SPropNames,
	                                    unsigned int,
										std::vector<SimulationConfigConst::config_storage_t>,
	                                    std::vector<SimulationConfigConst::config_storage_t>,
										std::string >;
};

/*!
 * \brief Contains information regarding single simulation
 */
class SimulationConfig
        : public SimulationConfigConst,
          public SimulationConfigConst::SProps
{
	public:
		SimulationConfig(const nlohmann::json &configuration);

		const std::string &name() const;
		std::string &name();

		unsigned int simulationTimeOut() const;
		unsigned int &simulationTimeOut();

		const tf_configs_t &engineConfigs() const;
		tf_configs_t &engineConfigs();

		const tf_configs_t &transceiverFunctionConfigs() const;
		tf_configs_t &transceiverFunctionConfigs();

	private:
};

using SimulationConfigSharedPtr = std::shared_ptr<SimulationConfig>;
using SimulationConfigConstSharedPtr = std::shared_ptr<const SimulationConfig>;

#endif // SIMULATION_CONFIG_H
