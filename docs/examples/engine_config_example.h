#ifndef ENGINE_CONFIG_EXAMPLE_H
#define ENGINE_CONFIG_EXAMPLE_H

#include "nrp_general_library/config/engine_config.h"

class EngineConfigExample
        : public EngineConfig<EngineConfigExample, PropNames<"Address", "Port">, std::string, uint16_t>
{
	public:
		static constexpr FixedString DefEngineType = "example_engine";
		static constexpr std::string_view DefEngineName = "ExampleEngine";

		static constexpr std::string_view DefEngineProcCmd = "ExampleServerExecutable";

		EngineConfigExample(EngineConfigConst::config_storage_t &config)
		    : EngineConfig(config.Data, "http://localhost", 8080)
		{}

		std::string &address()
		{	return this->getPropertyByName<"Address">();	}

		const std::string &address() const
		{	return this->getPropertyByName<"Address">();	}

		uint16_t &port()
		{	return this->getPropertyByName<"Port">();	}

		const std::string &port() const
		{	return this->getPropertyByName<"Port">();	}
};

#endif // ENGINE_CONFIG_EXAMPLE_H
