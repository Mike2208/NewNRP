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
