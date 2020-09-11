#include <gtest/gtest.h>

#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

using namespace testing;

using std::string_view_literals::operator""sv;
namespace python = boost::python;

struct TestPythonDictPropertySerializer
        : public PropertyTemplate<TestPythonDictPropertySerializer, PropNames<"string", "int">, std::string, int>
{
	using property_template_t = PropertyTemplate<TestPythonDictPropertySerializer, PropNames<"string", "int">, std::string, int>;

	template<class ...T>
	TestPythonDictPropertySerializer(boost::python::dict data, T &&...properties)
	    : PropertyTemplate(PythonDictPropertySerializer<PropertyTemplate>::readProperties(data, std::forward<T>(properties)...))
	{}

	template<class ...T>
	TestPythonDictPropertySerializer(T &&...properties)
	    : PropertyTemplate(std::forward<T>(properties)...)
	{}
};

TEST(PythonDictPropertySerializerTest, Serialization)
{
	Py_Initialize();
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int testInt = 10;
	const std::string testStr = "value";
	TestPythonDictPropertySerializer props(testStr, testInt);

	// Test serialization
	boost::python::dict serializedData = PythonDictPropertySerializer<TestPythonDictPropertySerializer>::serializeProperties(props, python::dict());
	boost::python::incref(serializedData.ptr());
	ASSERT_EQ(python::extract<int>(serializedData[intName.m_data]), testInt);
	const std::string testSerializedStr = python::extract<std::string>(serializedData[stringName.m_data]);
	ASSERT_STREQ(testSerializedStr.data(), testStr.data());

	ASSERT_EQ(python::len(serializedData), 2);
}


TEST(PythonDictPropertySerializerTest, Deserialization)
{
	Py_Initialize();
	static constexpr FixedString intName = "int";
	static constexpr FixedString stringName = "string";

	const int testInt = 10;
	const std::string testStr = "value";

	// Test deserialization
	python::dict data;
	python::incref(data.ptr());
	data[intName.m_data] = testInt;
	data[stringName.m_data] = testStr;
	TestPythonDictPropertySerializer prop(data);

	ASSERT_EQ((prop.getPropertyByName<intName, int>()), testInt);
	ASSERT_STREQ((prop.getPropertyByName<stringName, std::string>().data()), testStr.data());

	ASSERT_EQ(TestPythonDictPropertySerializer::NumProperties, 2);

	// Test defaults
	python::delitem(data, python::str(stringName.m_data));
	ASSERT_THROW(TestPythonDictPropertySerializer propThrow(data), std::exception);		// Should throw if python object is missing property and no default is specified

	const std::string defStr = "default";
	TestPythonDictPropertySerializer prop2(data, defStr, testInt);
	ASSERT_EQ((prop2.getPropertyByName<intName, int>()), testInt);
	ASSERT_STREQ((prop2.getPropertyByName<stringName, std::string>().data()), defStr.data());
}
