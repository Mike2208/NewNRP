#ifndef GAZEBO_GRPC_CONFIG_H
#define GAZEBO_GRPC_CONFIG_H

#include "nrp_grpc_engine_protocol/config/engine_grpc_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

struct GazeboGrpcConfigConst
{
	/*!
	 * \brief GazeboGrpcConfig Type
	 */
	static constexpr FixedString ConfigType = "GazeboGrpcConfig";

	/*!
	 * \brief Plugins to load into gazebo
	 */
	static constexpr FixedString GazeboPlugins = "GazeboPlugins";
	static const std::vector<std::string> DefGazeboPlugins;

	static constexpr FixedString GazeboRNGSeed = "GazeboRNGSeed";
	static const size_t DefGazeboRNGSeed;

	/*!
	 * \brief Gazebo Start Parameters Argument for plugins
	 */
	static constexpr std::string_view GazeboPluginArg = "-s";

	/*!
	 * \brief Gazebo Start Parameters Argument for random seed value
	 */
	static constexpr std::string_view GazeboRNGSeedArg = "--seed";

	/*!
	 * \brief Maximum time (in seconds) to wait for the NRPCommunicatioPlugin to load the world file, and for the world sdf to be loaded. 0 means it will wait forever
	 */
	static constexpr FixedString MaxWorldLoadTime = "WorldLoadTime";
	static constexpr unsigned int DefMaxWorldLoadTime = 20;

	/*!
	 * \brief Gazebo SDF World file
	 */
	static constexpr FixedString GazeboWorldFile = "GazeboWorldFile";

	using GPropNames = PropNames<GazeboPlugins, GazeboRNGSeed, MaxWorldLoadTime, GazeboWorldFile>;
};

/*!
 *  \brief Configuration for gazebo physics engine
 */
class GazeboGrpcConfig
        : public EngineGRPCConfig<GazeboGrpcConfig, GazeboGrpcConfigConst::GPropNames,
                                  std::vector<std::string>, std::size_t, unsigned int, std::string>,
          public GazeboGrpcConfigConst,
          public PtrTemplates<GazeboGrpcConfig>
{
	public:
		// Default engine values. Copied from EngineConfigConst
		static constexpr FixedString DefEngineType = "gazebo_grpc";
		static constexpr std::string_view DefEngineName = "gazebo_engine";
		//static const string_vector_t DefEngineProcEnvParams;
		static constexpr std::string_view DefEngineProcCmd = "/usr/bin/gzserver";
		//static const string_vector_t DefEngineProcStartParams;

		/*!
		 * \brief Constructor. Takes configuration data from the main SimulationConfig class. Will register itself with said class, so that anytime the configuration is saved, any changes made by GazeboGrpcConfig are passed along
		 * \param config Gazebo Configuration, taken from the SimulationConfig class
		 */
		GazeboGrpcConfig(EngineConfigConst::config_storage_t &config);

		std::string &gazeboWorldFile();
		const std::string &gazeboWorldFile() const;

		std::vector<std::string> &gazeboPlugins();
		const std::vector<std::string> &gazeboPlugins() const;

		size_t &gazeboRNGSeed();
		const size_t &gazeboRNGSeed() const;

		unsigned int &maxWorldLoadTime();
		const unsigned int &maxWorldLoadTime() const;

		string_vector_t allEngineProcEnvParams() const override;
		string_vector_t allEngineProcStartParams() const override;

	private:
		/*!
		 * \brief Checks that a given environment variable is properly formated. Should be something along the lines of "ENV_VARIABLE=VALUE", meaning there should be an '=' present in the string, and some character(s) before and after
		 * \param envVariable Environment variable to check
		 * \return Returns true if format is correct
		 */
		static bool checkEnvVariableFormat(const std::string &envVariable);

};

using GazeboGrpcConfigSharedPtr = GazeboGrpcConfig::shared_ptr;
using GazeboGrpcConfigConstSharedPtr = GazeboGrpcConfig::const_shared_ptr;

#endif // GAZEBO_GRPC_CONFIG_H
