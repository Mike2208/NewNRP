#include <gtest/gtest.h>

#include "nrp_general_library/utils/serializers/json_property_serializer.h"

using namespace testing;

using std::string_view_literals::operator""sv;

struct TestJSONPropertySerializer
        : public PropertyTemplate<TestJSONPropertySerializer, PropNames<"string", "int">, std::string, int>
{
	template<class ...T>
	TestJSONPropertySerializer(T &&...properties)
	    : PropertyTemplate(std::forward<T>(properties)...)
	{}

	template<class ...T>
	TestJSONPropertySerializer(const nlohmann::json &data, T &&...properties)
	    : PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, std::forward<T>(properties)...))
	{}
};

struct TestJSONPropertySerializerDefaults
        : public PropertyTemplate<TestJSONPropertySerializer, PropNames<"string", "int">, PropCfg<std::string,false>, int>
{
	template<class ...T>
	TestJSONPropertySerializerDefaults(T &&...properties)
	    : PropertyTemplate(std::forward<T>(properties)...)
	{}

	template<class ...T>
	TestJSONPropertySerializerDefaults(const nlohmann::json &data, T &&...properties)
	    : PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, std::forward<T>(properties)...))
	{}
};



TEST(JSONPropertySerializerTest, Serialization)
{
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int testInt = 10;
	const std::string testStr = "value";
	TestJSONPropertySerializer props(testStr, testInt);

	// Test serialization
	const nlohmann::json serializedData = JSONPropertySerializer<TestJSONPropertySerializer::property_template_t>::serializeProperties(props, nlohmann::json());
	ASSERT_EQ(serializedData[intName.m_data].get<int>(), testInt);
	ASSERT_STREQ(serializedData[stringName.m_data].get<std::string>().data(), testStr.data());

	ASSERT_EQ(serializedData.size(), 2);
}


TEST(JSONPropertySerializerTest, Deserialization)
{
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int testInt = 10;
	const std::string testStr = "value";

	// Test deserialization
	nlohmann::json data;
	data[intName.m_data] = testInt;
	data[stringName.m_data] = testStr;
	TestJSONPropertySerializer prop((const nlohmann::json&)data);

	ASSERT_EQ((prop.getPropertyByName<intName, int>()), testInt);
	ASSERT_STREQ((prop.getPropertyByName<stringName, std::string>().data()), testStr.data());

	ASSERT_EQ(TestJSONPropertySerializer::NumProperties, 2);

	// Test defaults
	data.erase(stringName.m_data);
	ASSERT_THROW(TestJSONPropertySerializer prop((const nlohmann::json&)data), std::exception);		// Should throw if json object is missing property and no default is specified

	const std::string defStr = "default";

	TestJSONPropertySerializer prop2((const nlohmann::json&)data, defStr);
	ASSERT_EQ((prop2.getPropertyByName<intName, int>()), testInt);
	ASSERT_STREQ((prop2.getPropertyByName<stringName, std::string>().data()), defStr.data());
}

TEST(JSONPropertySerializerTest, NoSerializeDefaults)
{
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int testInt = 10;
	const std::string testStr = "value";
	TestJSONPropertySerializerDefaults props(testStr, testInt);

	// Test serialization
	const nlohmann::json serializedData = JSONPropertySerializer<TestJSONPropertySerializerDefaults::property_template_t>::serializeProperties(props, nlohmann::json());
	ASSERT_EQ(serializedData[intName.m_data].get<int>(), testInt);
	ASSERT_EQ(serializedData.find(stringName.m_data), serializedData.end());

	ASSERT_EQ(serializedData.size(), 1);
}
