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

#ifndef GAZEBO_JSON_CONFIG_H
#define GAZEBO_JSON_CONFIG_H

#include "nrp_json_engine_protocol/config/engine_json_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

struct GazeboJSONConfigConst
{
	/*!
	 * \brief GazeboJSONConfig Type
	 */
	static constexpr FixedString ConfigType = "GazeboJSONConfig";

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
class GazeboJSONConfig
        : public EngineJSONConfig<GazeboJSONConfig, GazeboJSONConfigConst::GPropNames,
                                  std::vector<std::string>, std::size_t, unsigned int, std::string>,
          public GazeboJSONConfigConst,
          public PtrTemplates<GazeboJSONConfig>
{
	public:
		// Default engine values. Copied from EngineConfigConst
		static constexpr FixedString DefEngineType = "gazebo_json";
		static constexpr std::string_view DefEngineName = "gazebo_engine";
		//static const string_vector_t DefEngineProcEnvParams;
		static constexpr std::string_view DefEngineProcCmd = "/usr/bin/gzserver";
		//static const string_vector_t DefEngineProcStartParams;

		/*!
		 * \brief Constructor. Takes configuration data from the main SimulationConfig class. Will register itself with said class, so that anytime the configuration is saved, any changes made by GazeboJSONConfig are passed along
		 * \param config Gazebo Configuration, taken from the SimulationConfig class
		 */
		GazeboJSONConfig(EngineConfigConst::config_storage_t &config);

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

using GazeboJSONConfigSharedPtr = GazeboJSONConfig::shared_ptr;
using GazeboJSONConfigConstSharedPtr = GazeboJSONConfig::const_shared_ptr;

#endif // GAZEBO_JSON_CONFIG_H
