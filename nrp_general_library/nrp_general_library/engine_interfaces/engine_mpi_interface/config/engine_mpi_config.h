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
