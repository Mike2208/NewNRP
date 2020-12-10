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

#ifndef ENGINE_MPI_CONFIG_H
#define ENGINE_MPI_CONFIG_H

#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/process_launchers/launch_commands/mpi_spawn.h"

template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class EngineMPIConfig;

struct EngineMPIConfigConst
{
	static constexpr FixedString DefEngineLaunchCmd = MPISpawn::LaunchType;
};

template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class EngineMPIConfig
        : public EngineConfig<CONFIG, PROP_NAMES, PROPERTIES...>,
          public EngineMPIConfigConst
{
	public:
		static constexpr FixedString DefEngineLaunchCmd = EngineMPIConfigConst::DefEngineLaunchCmd;

		template<class ...T>
		EngineMPIConfig(EngineConfigConst::config_storage_t &config, T &&...properties)
		    : EngineConfig<CONFIG, PROP_NAMES, PROPERTIES...>(config.Data, std::forward<T>(properties)...)
		{
			// Add callback function to Simulation config so that any changes made by GazeboConfig will be properly stored
			config.Config = this;
		}
};

#endif // ENGINE_MPI_CONFIG_H
