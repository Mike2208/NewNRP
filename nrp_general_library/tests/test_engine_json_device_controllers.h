#ifndef TEST_ENGINGE_JSON_DEVICE_INTERFACE_H
#define TEST_ENGINGE_JSON_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

using dcm_t = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator>;

struct TestJSONDeviceInterface1
    : public DeviceInterface,
      public PropertyTemplate<TestJSONDeviceInterface1, PropNames<"data">, int>,
      public EngineJSONDeviceController
{
	static constexpr std::string_view TypeName = "test_type1";

	TestJSONDeviceInterface1(const DeviceIdentifier &devID, const nlohmann::json &data)
	    : DeviceInterface(devID),
	      PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, 0)),
	      EngineJSONDeviceController(devID)
	{}

	TestJSONDeviceInterface1(const std::string &name, const nlohmann::json &data)
	    : TestJSONDeviceInterface1(DeviceIdentifier(name, "test_engine1", TypeName.data()), data)
	{}

	virtual ~TestJSONDeviceInterface1() override = default;

	virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator &data) override
	{
		nlohmann::json retData({{"retrievedData1", {{data.key(), data.value()}}}});
		retData.update(PropertySerializer<nlohmann::json, property_template_t>::serializeProperties(*this));
		return retData;
	}

	virtual nlohmann::json handleDeviceData(const nlohmann::json &data) override
	{
		this->data() = data["data"];

		nlohmann::json retData({{"handledData1", data}});
		retData.update(PropertySerializer<nlohmann::json, property_template_t>::serializeProperties(*this));
		return retData;
	}

	const int &data() const
	{
		return this->getPropertyByName<"data", int>();
	}

	int &data()
	{
		return this->getPropertyByName<"data", int>();
	}

	static nlohmann::json convertDeviceToJSON(const TestJSONDeviceInterface1 &device)
	{
		auto json = dcm_t::serializeID(device);
		json.front() = JSONPropertySerializer<PropertyTemplate>::serializeProperties(device, std::move(json.front()));

		return json;
	}

	static TestJSONDeviceInterface1 convertJSONToDevice(const nlohmann::json::const_iterator &device)
	{
		const auto deviceID = dcm_t::getID(device);
		return TestJSONDeviceInterface1(deviceID.Name, device.value());
	}
};

struct TestJSONDeviceInterface2
    : public DeviceInterface,
      public PropertyTemplate<TestJSONDeviceInterface2, PropNames<"data">, int>,
      public EngineJSONDeviceController
{
	static constexpr std::string_view TypeName = "test_type2";

	TestJSONDeviceInterface2(const DeviceIdentifier &devID, const nlohmann::json &data)
	    : DeviceInterface(devID),
	      PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, 0)),
	      EngineJSONDeviceController(devID)
	{}

	TestJSONDeviceInterface2(const std::string &name, const nlohmann::json &data)
	    : TestJSONDeviceInterface2(DeviceIdentifier(name, "test_engine2", TypeName.data()), data)
	{}

	virtual ~TestJSONDeviceInterface2() override = default;

	virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator &data) override
	{
		nlohmann::json retData({{"retrievedData2", {{data.key(), data.value()}}}});
		retData.update(PropertySerializer<nlohmann::json, property_template_t>::serializeProperties(*this));
		return retData;
	}

	virtual nlohmann::json handleDeviceData(const nlohmann::json &data) override
	{
		this->data() = data["data"];

		nlohmann::json retData({{"handledData2", data}});
		retData.update(PropertySerializer<nlohmann::json, property_template_t>::serializeProperties(*this));
		return retData;
	}

	const int &data() const
	{
		return this->getPropertyByName<"data", int>();
	}

	int &data()
	{
		return this->getPropertyByName<"data", int>();
	}

	static nlohmann::json convertDeviceToJSON(const TestJSONDeviceInterface2 &device)
	{
		auto json = dcm_t::serializeID(device);
		json.front() = JSONPropertySerializer<PropertyTemplate>::serializeProperties(device, std::move(json.front()));

		return json;
	}

	static TestJSONDeviceInterface2 convertJSONToDevice(const nlohmann::json::const_iterator &device)
	{
		const auto deviceID = dcm_t::getID(device);
		return TestJSONDeviceInterface2(deviceID.Name, device.value());
	}

};

struct TestJSONDeviceInterfaceThrow
    : public DeviceInterface,
      public PropertyTemplate<TestJSONDeviceInterfaceThrow, PropNames<"data">, int>,
      public EngineJSONDeviceController
{
	static constexpr std::string_view TypeName = "test_type_throw";

	TestJSONDeviceInterfaceThrow(const DeviceIdentifier &devID, const nlohmann::json &data)
	    : DeviceInterface(devID),
	      PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, 0)),
	      EngineJSONDeviceController(devID)
	{}

	TestJSONDeviceInterfaceThrow(const std::string &name, const nlohmann::json &data)
	    : TestJSONDeviceInterfaceThrow(DeviceIdentifier(name, "test_engine", TypeName.data()), data)
	{}

	virtual ~TestJSONDeviceInterfaceThrow() override = default;

	virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator&) override
	{
		throw std::invalid_argument("Error");
	}

	virtual nlohmann::json handleDeviceData(const nlohmann::json&) override
	{
		throw std::domain_error("Error");
	}

	static nlohmann::json convertDeviceToJSON(const TestJSONDeviceInterfaceThrow &device)
	{
		auto json = dcm_t::serializeID(device);
		json.front() = JSONPropertySerializer<PropertyTemplate>::serializeProperties(device, std::move(json.front()));

		return json;
	}

	static TestJSONDeviceInterfaceThrow convertJSONToDevice(const nlohmann::json::const_iterator &device)
	{
		const auto deviceID = dcm_t::getID(device);
		return TestJSONDeviceInterfaceThrow(deviceID.Name, device.value());
	}

};


#endif // TEST_ENGINGE_JSON_DEVICE_INTERFACE_H
