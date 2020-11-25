#ifndef NEST_CONFIG_H
#define NEST_CONFIG_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include "nrp_nest_json_engine/config/cmake_constants.h"

struct NestConfigConst
{
	/*!
	 * \brief NestConfig Type
	 */
	static constexpr FixedString ConfigType = "NestConfig";

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

	using NPropNames = PropNames<NestRNGSeed, NestInitFileName>;
};

class NestConfig
        : public EngineJSONConfig<NestConfig, NestConfigConst::NPropNames, size_t, std::string>,
          public PtrTemplates<NestConfig>,
          public NestConfigConst
{
	public:
		// Default engine values. Copies from EngineConfigConst
		static constexpr FixedString DefEngineType = "nest_json";
		static constexpr std::string_view DefEngineName = "nest_engine";
		//static const string_vector_t DefEngineProcEnvParams;
		static constexpr std::string_view DefEngineProcCmd = NRP_NEST_EXECUTABLE_PATH;
		//static const string_vector_t DefEngineProcStartParams;

		NestConfig(EngineConfigConst::config_storage_t &config);
		NestConfig(const nlohmann::json &data);

		size_t nestRNGSeed() const;
		size_t &nestRNGSeed();

		const std::string &nestInitFileName() const;
		std::string &nestInitFileName();

		string_vector_t allEngineProcEnvParams() const override;
		string_vector_t allEngineProcStartParams() const override;
};

using NestConfigSharedPtr = NestConfig::shared_ptr;
using NestConfigConstSharedPtr = NestConfig::const_shared_ptr;

#endif // NEST_CONFIG_H
