#include <gtest/gtest.h>

#include "nrp_general_library/config/config_storage.h"

using namespace testing;

struct TestConfig
        : public BaseConfig<nlohmann::json>
{
	nlohmann::json cfg;

	TestConfig()
	    : BaseConfig("TestConfig")
	{}

	nlohmann::json writeConfig() const override
	{	return this->cfg;	}
};

using TestConfigHolder = ConfigStorage<nlohmann::json>;

TEST(ConfigHolderTest, Constructor)
{
	const std::string jsonAttr("test");

	nlohmann::json cfg1;
	cfg1[jsonAttr] = "cfg1";

	TestConfigHolder ConfigStorage(cfg1);
	ASSERT_STREQ(ConfigStorage.Data[jsonAttr].get<std::string>().data(), cfg1[jsonAttr].get<std::string>().data());
}

TEST(ConfigHolderTest, Operators)
{
	const std::string jsonAttr("test");

	nlohmann::json cfg1;
	cfg1[jsonAttr] = "cfg1";

	nlohmann::json cfg2;
	cfg2[jsonAttr] = "cfg2_other";

	TestConfigHolder ConfigStorage(cfg1);
	ASSERT_STREQ(static_cast<const nlohmann::json&>(ConfigStorage)[jsonAttr].get<std::string>().data(), cfg1[jsonAttr].get<std::string>().data());

	TestConfig data;
	data.cfg = cfg2;
	ConfigStorage.Config = &data;
	ASSERT_STREQ(static_cast<const nlohmann::json&>(ConfigStorage)[jsonAttr].get<std::string>().data(), cfg2[jsonAttr].get<std::string>().data());
}
