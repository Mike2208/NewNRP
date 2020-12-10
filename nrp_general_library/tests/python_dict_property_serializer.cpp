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
