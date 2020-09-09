#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_interface.h"

using namespace testing;

using std::string_view_literals::operator""sv;

struct TestJSONDeviceInterface
        : public JSONDeviceInterface<TestJSONDeviceInterface, PropNames<"int", "string">, int, std::string>
{
	template<class ...T>
	TestJSONDeviceInterface(T &&...properties)
	    : JSONDeviceInterface(std::forward<T>(properties)...)
	{}

	virtual ~TestJSONDeviceInterface() override = default;
};

TEST(JSONDeviceInterfaceTest, Functions)
{
	nlohmann::json data;
	data["int"] = 6;

	// Test default value
	TestJSONDeviceInterface dev1(DeviceIdentifier("dev1", "type1", "engine"), data, 5, "data");
	ASSERT_EQ((dev1.getPropertyByName<"int", int>()), (data["int"].get<int>()));
	ASSERT_STREQ((dev1.getPropertyByName<"string", std::string>().data()), ("data"));

	// Assert exception throw when json is missing a property value and no default is passed
	ASSERT_THROW(TestJSONDeviceInterface(DeviceIdentifier("dev1", "type1", "engine"), data), std::exception);

	// Test json load
	data["string"] = "otherData";
	TestJSONDeviceInterface dev2(DeviceIdentifier("dev2", "type2", "engine"), data, 5, "data");
	ASSERT_EQ((dev2.getPropertyByName<"int", int>()), (data["int"].get<int>()));
	ASSERT_STREQ((dev2.getPropertyByName<"string", std::string>().data()), (data["string"].get<std::string>().data()));

	// Test serialization
	const nlohmann::json serializedData = dev2.serializeProperties(nlohmann::json());
	ASSERT_EQ(serializedData.size(), data.size());
	ASSERT_STREQ(serializedData["string"].get<std::string>().data(), data["string"].get<std::string>().data());
	ASSERT_EQ(serializedData["int"].get<int>(), data["int"].get<int>());
}
