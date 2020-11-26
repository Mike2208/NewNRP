#ifndef EXAMPLE_ENGINE_JSON_DEVICE_CONTROLLER_H
#define EXAMPLE_ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_example_json_engine/devices/example_device.h"

class ExampleEngineJSONDeviceControllerInterface
{
	public:
		ExampleEngineJSONDeviceControllerInterface(const DeviceIdentifier &devID);
		virtual ~ExampleEngineJSONDeviceControllerInterface() = default;

		virtual DeviceInterface::shared_ptr getDeviceInformation() = 0;
		virtual void handleDeviceData(DeviceInterface::unique_ptr &&data) = 0;

		constexpr const DeviceIdentifier &id() const
		{	return this->_id;	}

	private:
		DeviceIdentifier _id;
};

template<class DEVICE>
class ExampleEngineJSONDeviceController
        : public ExampleEngineJSONDeviceControllerInterface
{
	public:
		ExampleEngineJSONDeviceController(const DeviceIdentifier &devID)
		    : ExampleEngineJSONDeviceControllerInterface(devID)
		{}

		virtual ~ExampleEngineJSONDeviceController() override = default;

		virtual DeviceInterface::shared_ptr getDeviceInformation() override;
		virtual void handleDeviceData(DeviceInterface::unique_ptr &&data) override;
};

template<>
class ExampleEngineJSONDeviceController<ExampleDevice>
        : public ExampleEngineJSONDeviceControllerInterface
{
	public:
		ExampleEngineJSONDeviceController(const DeviceIdentifier &devID)
		    : ExampleEngineJSONDeviceControllerInterface(devID),
		      _dev(devID)
		{}

		virtual ~ExampleEngineJSONDeviceController() override;

		virtual DeviceInterface::shared_ptr getDeviceInformation() override
		{	return std::make_shared(new ExampleDevice(this->_dev));	}

		virtual void handleDeviceData(DeviceInterface::unique_ptr &&data) override
		{
			this->_dev = std::move(*dynamic_cast<ExampleDevice*>(data.release()));
		}

	private:
		ExampleDevice _dev;
};


#endif // EXAMPLE_ENGINE_JSON_DEVICE_CONTROLLER_H
