//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

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
