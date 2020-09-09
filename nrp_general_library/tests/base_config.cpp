#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "nrp_general_library/config/base_config.h"

using namespace testing;

struct TestBaseConfig
        : public BaseConfig<int>
{
		static constexpr std::string_view ConfigType = "TestConfig";

		int dat = 0;

		template<class ...T>
		TestBaseConfig(int _dat)
		    : BaseConfig(ConfigType.data())
		{	this->dat = _dat;	}

		int writeConfig() const override
		{	return this->dat;	}
};


TEST(BaseConfigTest, Constructor)
{
	const auto testInt = 5;
	TestBaseConfig testCfg(testInt);

	ASSERT_EQ(testCfg.configType(), TestBaseConfig::ConfigType);

	ASSERT_EQ(testCfg.dat, testInt);
	ASSERT_EQ(testCfg.writeConfig(), testInt);
}
