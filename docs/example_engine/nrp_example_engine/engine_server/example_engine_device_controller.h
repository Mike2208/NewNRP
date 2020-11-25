#ifndef EXAMPLE_ENGINE_DEVICE_CONTROLLER_H
#define EXAMPLE_ENGINE_DEVICE_CONTROLLER_H

#include "nrp_example_engine/devices/example_device.h"

class ExampleEngineDeviceControllerInterface
{
	public:
		ExampleEngineDeviceControllerInterface(const DeviceIdentifier &devID);
		virtual ~ExampleEngineDeviceControllerInterface() = default;

		virtual DeviceInterface::shared_ptr getDeviceInformation() = 0;
		virtual void handleDeviceData(DeviceInterface::unique_ptr &&data) = 0;

		constexpr const DeviceIdentifier &id() const
		{	return this->_id;	}

	private:
		DeviceIdentifier _id;
};

template<class DEVICE>
class ExampleEngineDeviceController
        : public ExampleEngineDeviceControllerInterface
{
	public:
		ExampleEngineDeviceController(const DeviceIdentifier &devID)
		    : ExampleEngineDeviceControllerInterface(devID)
		{}

		virtual ~ExampleEngineDeviceController() override = default;

		virtual DeviceInterface::shared_ptr getDeviceInformation() override;
		virtual void handleDeviceData(DeviceInterface::unique_ptr &&data) override;
};

template<>
class ExampleEngineDeviceController<ExampleDevice>
        : public ExampleEngineDeviceControllerInterface
{
	public:
		ExampleEngineDeviceController(const DeviceIdentifier &devID)
		    : ExampleEngineDeviceControllerInterface(devID),
		      _dev(devID)
		{}

		virtual ~ExampleEngineDeviceController() override;

		virtual DeviceInterface::shared_ptr getDeviceInformation() override
		{	return std::make_shared(new ExampleDevice(this->_dev));	}

		virtual void handleDeviceData(DeviceInterface::unique_ptr &&data) override
		{
			this->_dev = std::move(*dynamic_cast<ExampleDevice*>(data.release()));
		}

	private:
		ExampleDevice _dev;
};


#endif // EXAMPLE_ENGINE_DEVICE_CONTROLLER_H
