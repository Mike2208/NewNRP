#ifndef TEST_ENGINGE_JSON_DEVICE_INTERFACE_H
#define TEST_ENGINGE_JSON_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

using dcm_t = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator>;

struct TestJSONDevice1
    : public Device<TestJSONDevice1, "test_type1", PropNames<"data">, int>
{
	template<class ...DATA_T>
	TestJSONDevice1(DeviceIdentifier &&devID, DATA_T &&...data)
	    : Device(std::move(devID), std::forward<DATA_T>(data)...)
	{}

	template<class SERIALIZER_T>
	static TestJSONDevice1 deserialize(DeviceIdentifier &&devID, SERIALIZER_T &&data)
	{	return Device::deserialize(std::move(devID), data, 0);	}

	const int &data() const
	{	return this->getPropertyByName<"data">();	}

	int &data()
	{	return this->getPropertyByName<"data">();	}
};

struct TestJSONDevice1Controller
        : public EngineJSONDeviceController<TestJSONDevice1>
{
	public:
		template<class ...T>
		TestJSONDevice1Controller (T &&...params)
		    : EngineJSONDeviceController<TestJSONDevice1>(std::forward<T>(params)...)
		{}

		void handleDeviceDataCallback(TestJSONDevice1 &&data)
		{	this->_dev = std::move(data);	}

		const TestJSONDevice1 &getDeviceInformationCallback()
		{	return this->_dev;	}

	private:
		TestJSONDevice1 _dev;
};

struct TestJSONDevice2
    : public Device<TestJSONDevice2, "test_type2", PropNames<"data">, int>
{
	template<class ...DATA_T>
	TestJSONDevice2(DeviceIdentifier &&devID, DATA_T &&...data)
	    : Device(std::move(devID), std::forward<DATA_T>(data)...)
	{}

	template<class SERIALIZER_T>
	static TestJSONDevice2 deserialize(DeviceIdentifier &&devID, SERIALIZER_T &&data)
	{	return Device::deserialize(std::move(devID), data, 0);	}

	const int &data() const
	{	return this->getPropertyByName<"data">();	}

	int &data()
	{	return this->getPropertyByName<"data">();	}
};

struct TestJSONDevice2Controller
        : public EngineJSONDeviceController<TestJSONDevice2>
{
	public:
		template<class ...T>
		TestJSONDevice2Controller (T &&...params)
		    : EngineJSONDeviceController<TestJSONDevice2>(std::forward<T>(params)...)
		{}

		void handleDeviceDataCallback(TestJSONDevice2 &&data)
		{	this->_dev = std::move(data);	}

		const TestJSONDevice2 &getDeviceInformationCallback()
		{	return this->_dev;	}

	private:
		TestJSONDevice2 _dev;
};


struct TestJSONDeviceThrow
    : public Device<TestJSONDeviceThrow, "test_type_throw", PropNames<"data">, int>
{
	template<class ...DATA_T>
	TestJSONDeviceThrow(DeviceIdentifier &&devID, DATA_T &&...data)
	    : Device(std::move(devID), std::forward<DATA_T>(data)...)
	{}

	template<class SERIALIZER_T>
	static TestJSONDeviceThrow deserialize(DeviceIdentifier &&devID, SERIALIZER_T &&data)
	{	return Device::deserialize(std::move(devID), data, 0);	}

	const int &data() const
	{	return this->getPropertyByName<"data">();	}

	int &data()
	{	return this->getPropertyByName<"data">();	}
};

struct TestJSONDeviceThrowController
        : public EngineJSONDeviceController<TestJSONDeviceThrow>
{
	public:
		template<class ...T>
		TestJSONDeviceThrowController(T &&...params)
		    : EngineJSONDeviceController<TestJSONDeviceThrow>(std::forward<T>(params)...)
		{}

		void handleDeviceDataCallback(TestJSONDeviceThrow &&)
		{	throw std::invalid_argument("Error");;	}

		const TestJSONDeviceThrow &getDeviceInformationCallback()
		{	throw std::domain_error("Error");	}

	private:
		TestJSONDeviceThrow _dev;
};

#endif // TEST_ENGINGE_JSON_DEVICE_INTERFACE_H
