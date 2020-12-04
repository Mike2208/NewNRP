#ifndef TEST_ENGINGE_JSON_DEVICE_INTERFACE_H
#define TEST_ENGINGE_JSON_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_json_engine_protocol/device_interfaces/json_device_conversion_mechanism.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"
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
	static auto deserializeProperties(SERIALIZER_T &&data)
	{	return Device::deserializeProperties(std::forward<SERIALIZER_T>(data), 0);	}

	const int &data() const
	{	return this->getPropertyByName<"data">();	}

	int &data()
	{	return this->getPropertyByName<"data">();	}
};

struct TestJSONDevice1Controller
        : public EngineDeviceController<nlohmann::json, TestJSONDevice1>
{
	public:
		template<class ...T>
		TestJSONDevice1Controller (T &&...params)
		    : EngineDeviceController<nlohmann::json, TestJSONDevice1>(std::forward<T>(params)...),
		      _dev(DeviceIdentifier(*this))
		{}

		void handleDeviceDataCallback(TestJSONDevice1 &&data)
		{	this->_dev = std::move(data);	}

		const TestJSONDevice1 *getDeviceInformationCallback()
		{	return &(this->_dev);	}

		constexpr const auto &data() const
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
	static auto deserializeProperties(SERIALIZER_T &&data)
	{	return Device::deserializeProperties(std::forward<SERIALIZER_T>(data), 0);	}

	const int &data() const
	{	return this->getPropertyByName<"data">();	}

	int &data()
	{	return this->getPropertyByName<"data">();	}
};

struct TestJSONDevice2Controller
        : public EngineDeviceController<nlohmann::json, TestJSONDevice2>
{
	public:
		template<class ...T>
		TestJSONDevice2Controller (T &&...params)
		    : EngineDeviceController<nlohmann::json, TestJSONDevice2>(std::forward<T>(params)...),
		      _dev(DeviceIdentifier(*this))
		{}

		void handleDeviceDataCallback(TestJSONDevice2 &&data)
		{	this->_dev = std::move(data);	}

		const TestJSONDevice2 *getDeviceInformationCallback()
		{	return &(this->_dev);	}

		constexpr const auto &data() const
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
	static auto deserializeProperties(SERIALIZER_T &&data)
	{	return Device::deserializeProperties(std::forward<SERIALIZER_T>(data), 0);	}

	const int &data() const
	{	return this->getPropertyByName<"data">();	}

	int &data()
	{	return this->getPropertyByName<"data">();	}
};

struct TestJSONDeviceThrowController
        : public EngineDeviceController<nlohmann::json, TestJSONDeviceThrow>
{
	public:
		template<class ...T>
		TestJSONDeviceThrowController(T &&...params)
		    : EngineDeviceController<nlohmann::json, TestJSONDeviceThrow>(std::forward<T>(params)...),
		      _dev(DeviceIdentifier(*this))
		{}

		void handleDeviceDataCallback(TestJSONDeviceThrow &&)
		{	throw std::domain_error("Error");;	}

		const TestJSONDeviceThrow *getDeviceInformationCallback()
		{	throw std::invalid_argument("Error");	}

	private:
		TestJSONDeviceThrow _dev;
};

#endif // TEST_ENGINGE_JSON_DEVICE_INTERFACE_H
