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

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "nrp_general_library/config/config_formats/json_config_properties.h"
#include "nrp_general_library/utils/ptr_templates.h"
#include "nrp_simulation/config/cmake_conf.h"

#include <filesystem>

class ServerConfig;

struct ServerConfigConst
{
	/*!
	 * \brief ServerConfig Type
	 */
	static constexpr FixedString ConfigType = "ServerConfig";

	/*!
	 * \brief Process Launcher Type. Can be used to change the way engine processes are started
	 */
	static constexpr FixedString ProcessLauncherType = "ProcessLauncher";
	static constexpr std::string_view DefProcessLauncherType = "Basic";

	/*!
	 * \brief Address to NRPServer REST server
	 */
	static constexpr FixedString ServerAddress = "ServerAddress";
	static constexpr std::string_view DefServerAddress = "localhost:8990";

	/*!
	 * \brief Server Working Directory
	 */
	static constexpr FixedString ServerWorkingDirectory = "ServerWorkingDirectory";
	static constexpr std::string_view DefServerWorkingDirectory = NRP_SERVER_DEFAULT_WORKING_DIR;

	/*!
	 * \brief Maximum number of experiments. Set to -1 if unlimited
	 */
	static constexpr FixedString MaxNumExperiments = "MaxNumExperiments";
	static constexpr int32_t DefMaxNumExperiments = NRP_SERVER_DEF_MAX_NUM_EXPERIMENTS;

	/*!
	 * \brief Server Timestep.
	 * Defines how far the simulation should progress in between processing client requests. If the server is responding too slo
	 */
	static constexpr FixedString ServerTimestep = "ServerTimestep";
	static constexpr float DefServerTimestep = 0.01f;

	/*!
	 * \brief Number of threads that handle requests from NRPServer
	 */
	static constexpr uint16_t SimServerThreadPoolSize = 10;

	/*!
	 * \brief Directory in ServerWorkingDirectory where uncompressed experiments are stored
	 */
	static constexpr std::string_view ServerExperimentDir = "experiments";

	/*!
	 * \brief Default Username. Used when no user is supplied by a REST request
	 */
	static constexpr std::string_view DefaultUsername = "default";

	/*!
	 * \brief Default Experiment Name. Used when no experiment name is supplied by a REST request
	 */
	static constexpr std::string_view DefaultExperimentName = "default";

	using SPropNames = PropNames<ProcessLauncherType, ServerAddress, ServerWorkingDirectory, MaxNumExperiments, ServerTimestep>;
	using SPropConfig = JSONConfigProperties<ServerConfig, SPropNames, std::string, std::string, std::filesystem::path, int32_t, float>;
};

/*!
 * \brief Class that manages the server configuration
 */
class ServerConfig
        : public ServerConfigConst,
          public PtrTemplates<ServerConfig>,
          public ServerConfigConst::SPropConfig
{
	public:
		ServerConfig(const nlohmann::json &config);

		std::string &processLauncherType();
		const std::string &processLauncherType() const;

		std::string &serverAddress();
		const std::string &serverAddress() const;

		std::filesystem::path &serverWorkingDirectory();
		const std::filesystem::path &serverWorkingDirectory() const;

		std::filesystem::path serverExperimentDirectory() const;

		int32_t &maxNumExperiments();
		int32_t maxNumExperiments() const;

		float &serverTimestep();
		float serverTimestep() const;

	private:
};

using ServerConfigSharedPtr = ServerConfig::shared_ptr;
using ServerConfigConstSharedPtr = ServerConfig::const_shared_ptr;

#endif // SERVER_CONFIG_H
