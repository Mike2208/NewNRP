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

#ifndef NEST_SERVER_CONFIG_H
#define NEST_SERVER_CONFIG_H

#include "nrp_json_engine_protocol/config/engine_json_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include "nrp_nest_server_engine/config/cmake_constants.h"

struct NestServerConfigConst
{
	/*!
	 * \brief NestServerConfig Type
	 */
	static constexpr FixedString ConfigType = "NestServerConfig";

	/*!
	 * \brief Nest RNG seed
	 */
	static constexpr FixedString NestRNGSeed = "NestRNGSeed";
	static const size_t DefNestRNGSeed;

	/*!
	 * \brief Nest Init File (contains a python script that sets up the neural network, as well as any other initialization routines the user whishes to perform)
	 */
	static constexpr FixedString NestInitFileName = "NestInitFileName";
	static constexpr std::string_view DefNestInitFileName = "";

	/*!
	 * \brief Nest Server Host
	 */
	static constexpr FixedString NestServerHost = "NestServerHost";
	static constexpr std::string_view DefNestServerHost = "localhost";

	/*!
	 * \brief Nest Server Port
	 */
	static constexpr FixedString NestServerPort = "NestServerPort";

	/*!
	 * \brief The NestServerConfig will look for an unbound port as default. This is the port number at which to start the search
	 */
	static constexpr uint16_t PortSearchStart = 5000;

	/*!
	 * \brief Python Path to Nest. Automatically generated via cmake on installation
	 */
	static constexpr std::string_view NestPythonPath = "PYTHONPATH=" NRP_PYNEST_PATH ":$PYTHONPATH";

	/*!
	 * \brief Path to NRP Nest Server Executable. Automatically generated via cmake on installation
	 */
	static constexpr std::string_view NestExecutablePath = "PATH=$PATH:" NRP_NEST_BIN_PATH;

	/*!
	 * \brief Argument to pass RNG seed argument to Nest
	 */
	static constexpr std::string_view NestRNGSeedArg = "--nrprng";

	/*!
	 * \brief After the server executes the init file, this status flag will either be 1 for success or 0 for fail. If the execution fails, a JSON message with more details will be passed as well (under InitFileErrorMsg).
	 */
	static constexpr std::string_view InitFileExecStatus = "InitExecStatus";

	/*!
	 * \brief After the server executes the init file, the parsed devMap will be passed back with this param
	 */
	static constexpr std::string_view InitFileParseDevMap = "InitFileParseDevMap";

	/*!
	 * \brief If the init file could not be parsed, the python error message will be stored under this JSON property name
	 */
	static constexpr std::string_view InitFileErrorMsg = "Message";

	using NPropNames = PropNames<NestRNGSeed, NestInitFileName, NestServerHost, NestServerPort>;
};

class NestServerConfig
        : public EngineJSONConfig<NestServerConfig, NestServerConfigConst::NPropNames, size_t, std::string, std::string, uint16_t>,
          public PtrTemplates<NestServerConfig>,
          public NestServerConfigConst
{
	public:
		// Default engine values. Copies from EngineConfigConst
		static constexpr FixedString DefEngineType = "nest_server";
		static constexpr FixedString DefEngineName = "nest_server_engine";
		//static const string_vector_t DefEngineProcEnvParams;
		static constexpr std::string_view DefEngineProcCmd = NRP_NEST_SERVER_EXECUTABLE_PATH;
		//static const string_vector_t DefEngineProcStartParams;

		NestServerConfig(EngineConfigConst::config_storage_t &config);
		NestServerConfig(const nlohmann::json &data);

		size_t nestRNGSeed() const;
		size_t &nestRNGSeed();

		const std::string &nestInitFileName() const;
		std::string &nestInitFileName();

		const std::string &nestServerHost() const;
		std::string &nestServerHost();

		const uint16_t &nestServerPort() const;
		uint16_t &nestServerPort();

		string_vector_t allEngineProcEnvParams() const override;
		string_vector_t allEngineProcStartParams() const override;

	private:
		static uint16_t NextDefPort;
};

using NestServerConfigSharedPtr = NestServerConfig::shared_ptr;
using NestServerConfigConstSharedPtr = NestServerConfig::const_shared_ptr;

#endif // NEST_SERVER_CONFIG_H
