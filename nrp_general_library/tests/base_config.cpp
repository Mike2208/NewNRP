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
