#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"

using namespace testing;

struct TestEngineJSONConfig
        : public EngineJSONConfig<TestEngineJSONConfig, PropNames<> >
{
	static constexpr FixedString ConfigType = "TestEngineConfig";


	template<class ...T>
	TestEngineJSONConfig(T &&...properties)
	    : EngineJSONConfig(std::forward<T>(properties)...)
	{}
};

TEST(EngineJSONConfigTest, Functions)
{
	TestEngineJSONConfig ConfigStorage("engineName", nlohmann::json());

	ASSERT_STREQ(ConfigStorage.engineServerAddress().data(), TestEngineJSONConfig::DefEngineServerAddress.data());
}
