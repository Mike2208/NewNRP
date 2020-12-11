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

#ifndef PYTHON_CONFIG_H
#define PYTHON_CONFIG_H

#include "nrp_json_engine_protocol/config/engine_json_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include "nrp_python_json_engine/config/cmake_constants.h"

struct PythonConfigConst
{
	/*!
	 * \brief PythonConfig Type
	 */
	static constexpr FixedString ConfigType = "PythonConfig";

	/*!
	 * \brief Python File (contains the python engine script)
	 */
	static constexpr FixedString PythonFileName = "PythonFileName";
	static constexpr std::string_view DefPythonFileName = "";

	/*!
	 * \brief After the server executes the init file, this status flag will either be 1 for success or 0 for fail. If the execution fails, a JSON message with more details will be passed as well (under InitFileErrorMsg).
	 */
	static constexpr std::string_view InitFileExecStatus = "InitExecStatus";

	/*!
	 * \brief If the init file could not be parsed, the python error message will be stored under this JSON property name
	 */
	static constexpr std::string_view InitFileErrorMsg = "Message";

	using PPropNames = PropNames<PythonFileName>;
};

class PythonConfig
        : public EngineJSONConfig<PythonConfig, PythonConfigConst::PPropNames, std::string>,
          public PtrTemplates<PythonConfig>,
          public PythonConfigConst
{
	public:
		// Default engine values. Copies from EngineConfigConst
		static constexpr FixedString DefEngineType = "python_json";
		static constexpr std::string_view DefEngineName = "python_engine";
		//static const string_vector_t DefEngineProcEnvParams;
		static constexpr std::string_view DefEngineProcCmd = NRP_PYTHON_EXECUTABLE_PATH;
		//static const string_vector_t DefEngineProcStartParams;

		PythonConfig(EngineConfigConst::config_storage_t &config);
		PythonConfig(const nlohmann::json &data);

		const std::string &pythonFileName() const;
		std::string &pythonFileName();

		//string_vector_t allEngineProcEnvParams() const override;
		string_vector_t allEngineProcStartParams() const override;
};

using PythonConfigSharedPtr = PythonConfig::shared_ptr;
using PythonConfigConstSharedPtr = PythonConfig::const_shared_ptr;

#endif // PYTHON_CONFIG_H
