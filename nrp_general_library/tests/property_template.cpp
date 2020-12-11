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

#include "nrp_general_library/utils/property_template.h"

using namespace testing;

using std::string_view_literals::operator""sv;

struct TestPropertyTemplate
        : public PropertyTemplate<TestPropertyTemplate, PropNames<"int", "string">, int, std::string>
{
	using property_template_t = PropertyTemplate<TestPropertyTemplate, PropNames<"int", "string">, int, std::string>;

	template<class ...T>
	TestPropertyTemplate(T &&...properties)
	    : property_template_t(std::forward<T>(properties)...)
	{}
};

struct TestPropertyConfig
        : public PropertyTemplate<TestPropertyTemplate, PropNames<"int", "string">, PropCfg<int, false>, std::string>
{
	using property_template_t = PropertyTemplate<TestPropertyTemplate, PropNames<"int", "string">, PropCfg<int, false>, std::string>;

	template<class ...T>
	TestPropertyConfig(T &&...properties)
	    : property_template_t(std::forward<T>(properties)...)
	{}
};

TEST(PropertyTemplateTest, Tests)
{
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int testInt = 10;
	const std::string testStr = "value";

	// Test constructor with default values
	TestPropertyTemplate prop(testInt, testStr);

	ASSERT_EQ(TestPropertyTemplate::NumProperties, 2);

	// Test properties
	ASSERT_EQ(prop.getProperty<0>(), testInt);
	ASSERT_STREQ(prop.getProperty<1>().data(), testStr.data());

	ASSERT_STREQ(prop.getName<0>().data(), intName.m_data);
	ASSERT_STREQ(prop.getName<1>().data(), stringName.m_data);

	// Test Property Name retrieval
	ASSERT_EQ((prop.getPropertyByName<intName, int>()), testInt);
	ASSERT_STREQ((prop.getPropertyByName<stringName, std::string>().data()), testStr.data());

	const int testChangeInt = -1;
	prop.getPropertyByName<intName, int>() = testChangeInt;
	ASSERT_EQ((prop.getPropertyByName<intName, int>()), testChangeInt);

	const std::string testChangeStr = "change";
	prop.getProperty<1>() = testChangeStr;
}

TEST(PropertyTemplateTest, TestOptions)
{
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int defInt = 10;
	const std::string testStr = "value";

	// Test constructor with default values
	TestPropertyConfig prop(defInt, testStr);

	ASSERT_EQ(TestPropertyTemplate::NumProperties, 2);

	// Test properties
	ASSERT_EQ(prop.getProperty<0>(), defInt);
	ASSERT_STREQ(prop.getProperty<1>().data(), testStr.data());

	ASSERT_STREQ(prop.getName<0>().data(), intName.m_data);
	ASSERT_STREQ(prop.getName<1>().data(), stringName.m_data);

	// Test Property Name retrieval
	ASSERT_EQ((prop.getPropertyByName<intName, int>()), defInt);
	ASSERT_STREQ((prop.getPropertyByName<stringName, std::string>().data()), testStr.data());

	const int testChangeInt = -1;
	prop.getPropertyByName<intName, int>() = testChangeInt;
	ASSERT_EQ((prop.getPropertyByName<intName, int>()), testChangeInt);

	const std::string testChangeStr = "change";
	prop.getProperty<1>() = testChangeStr;

	ASSERT_EQ(prop.isDefaultWritable<0>(), false);
	ASSERT_EQ(prop.isDefaultWritable<1>(), true);

	ASSERT_EQ(prop.isDefaultWritable<"int">(), false);
	ASSERT_EQ(prop.isDefaultWritable<"string">(), true);

	ASSERT_EQ(prop.getDefaultValue<0>(), defInt);
	ASSERT_EQ(prop.getDefaultValue<"int">(), defInt);
}
