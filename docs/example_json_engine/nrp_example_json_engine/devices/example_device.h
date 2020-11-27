#ifndef EXAMPLE_DEVICE_H
#define EXAMPLE_DEVICE_H

#include "nrp_general_library/device_interface/device.h"

class ExampleDevice
        : public Device<ExampleDevice, "ExampleDevice", PropNames<"number", "string">, int, std::string>
{
	public:
		ExampleDevice(DeviceIdentifier &&devID, property_template_t &&props = property_template_t(0, "empty"))
		    : Device(std::move(devID), std::move(props))
		{}

		template<class DESERIALIZER_T>
		ExampleDevice deserialize(DeviceIdentifier &&devID, DESERIALIZER_T &&data)
		{	return Device::template deserialize(std::move(devID), std::forward<DESERIALIZER_T>(data), 0, "empty");	}

		const int &number() const;
		int &number();

		const std::string &string() const;
		std::string &string();
};

#endif // EXAMPLE_DEVICE_H
