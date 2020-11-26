#ifndef EXAMPLE_JSON_CONFIG_H
#define EXAMPLE_JSON_CONFIG_H

#include "nrp_example_json_engine/config/cmake_constants.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_config.h"


class ExampleJSONConfig
        : public EngineJSONConfig<ExampleJSONConfig>
{
	public:
		// Default engine values
		static constexpr FixedString ConfigType = "ExampleJSONConfig";
		static constexpr FixedString DefEngineType = "example_json_engine";
		static constexpr std::string_view DefEngineName = "ExampleJSONEngine";
		// static constexpr FixedString DefEngineLaunchCmd = "Default";

		static constexpr std::string_view DefEngineProcCmd = NRP_EXAMPLE_EXECUTABLE_PATH;

		ExampleJSONConfig(EngineConfigConst::config_storage_t &config)
		    : ExampleJSONConfig(config, "http://127.0.0.1", 8080)
		{}

		virtual ~ExampleJSONConfig() override;
};

#endif // EXAMPLE_JSON_CONFIG_H
