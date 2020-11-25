#ifndef EXAMPLE_CONFIG_H
#define EXAMPLE_CONFIG_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include "nrp_example_engine/config/cmake_constants.h"

class ExampleConfig
        : public EngineConfig<ExampleConfig, PropNames<"Address", "Port">, std::string, uint16_t>
{
	public:
		// Default engine values
		static constexpr FixedString ConfigType = "ExampleConfig";
		static constexpr FixedString DefEngineType = "example_engine";
		static constexpr std::string_view DefEngineName = "ExampleEngine";
		// static constexpr FixedString DefEngineLaunchCmd = "Default";

		static constexpr std::string_view DefEngineProcCmd = NRP_EXAMPLE_EXECUTABLE_PATH;

		ExampleConfig(EngineConfigConst::config_storage_t &config)
		    : ExampleConfig(config, "http://127.0.0.1", 8080)
		{}

		virtual ~ExampleConfig() override;

		const std::string &address() const
		{	return this->getPropertyByName<"Address">();	}
		std::string &address()
		{	return this->getPropertyByName<"Address">();	}

		const uint16_t &port() const
		{	return this->getPropertyByName<"Port">();	}
		uint16_t &port()
		{	return this->getPropertyByName<"Port">();	}
};

#endif // EXAMPLE_CONFIG_H
