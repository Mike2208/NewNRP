#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_interface.h"

using namespace testing;

using dcm_t = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator>;
using std::string_view_literals::operator""sv;

struct TestJSONDeviceInterface
        : public JSONDeviceInterface<TestJSONDeviceInterface, PropNames<"int", "string">, int, std::string>
{
	template<class ...T>
	TestJSONDeviceInterface(T &&...properties)
	    : JSONDeviceInterface(std::forward<T>(properties)...)
	{}

	TestJSONDeviceInterface(const DeviceIdentifier &id, const nlohmann::json &data)
	    : JSONDeviceInterface(id, data)
	{}
};

TEST(JSONDeviceMethodsTest, IDFunctions)
{
	nlohmann::json data;
	data["int"] = 6;
	data["string"] = "otherData";
	TestJSONDeviceInterface dev1(DeviceIdentifier("dev2", "type2", "engine"), data);

	// Test serialization
	const nlohmann::json serializedData = dcm_t::serializeID(dev1.id());
	ASSERT_EQ(serializedData.size(), 1);
	ASSERT_EQ(serializedData.front().size(), 2);
	ASSERT_STREQ(serializedData.begin().key().data(), dev1.name().data());
	ASSERT_STREQ(serializedData.front().at(dcm_t::JSONTypeID.data()).get<std::string>().data(), dev1.type().data());
	ASSERT_STREQ(serializedData.front().at(dcm_t::JSONEngineNameID.data()).get<std::string>().data(), dev1.engineName().data());

	// Test deserialization
	DeviceIdentifier deserializedID = dcm_t::getID(serializedData.begin());
	ASSERT_EQ(dev1.id(), deserializedID);

	// Test exception throwing
	nlohmann::json empty;
	empty["test"] = nlohmann::json();
	ASSERT_THROW(dcm_t::getID(empty.begin()), std::exception);
}

TEST(JSONDeviceMethodsTest, ConversionFunctions)
{
	nlohmann::json data;
	data["int"] = 6;
	data["string"] = "otherData";
	TestJSONDeviceInterface dev1(DeviceIdentifier("dev2", "type2", "engine"), data);

	// Test serialization
	const nlohmann::json serializedData = dcm_t::serialize(dev1);
	ASSERT_EQ(serializedData.size(), 1);
	ASSERT_EQ(serializedData.front().size(), 4);
	ASSERT_STREQ(serializedData.begin().key().data(), dev1.name().data());
	ASSERT_STREQ(serializedData.front().at(dcm_t::JSONTypeID.data()).get<std::string>().data(), dev1.type().data());
	ASSERT_STREQ(serializedData.front().at(dcm_t::JSONEngineNameID.data()).get<std::string>().data(), dev1.engineName().data());
	ASSERT_EQ(serializedData.front().at("int").get<int>(), (dev1.getPropertyByName<"int", int>()));
	ASSERT_STREQ(serializedData.front().at("string").get<std::string>().data(), (dev1.getPropertyByName<"string", std::string>().data()));

	auto deserializedDev = dcm_t::deserialize<TestJSONDeviceInterface>(serializedData.begin());
	ASSERT_EQ(dev1.id(), deserializedDev.id());
	ASSERT_EQ((dev1.getPropertyByName<"int", int>()), (deserializedDev.getPropertyByName<"int", int>()));
	ASSERT_STREQ((dev1.getPropertyByName<"string", std::string>().data()), (deserializedDev.getPropertyByName<"string", std::string>().data()));
}
