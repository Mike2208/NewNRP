#include "nrp_nest_json_engine/devices/nest_device.h"

NestDevice::NestDevice(DeviceIdentifier &&devID, const boost::python::object &data)
    : NestDevice(std::move(devID), property_template_t(defaultPyObject(data)))
{}

NestDevice::NestDevice(DeviceIdentifier &&devID, property_template_t &&props)
    : Device(std::move(devID), std::move(props))
{}


const boost::python::object &NestDevice::data() const
{
	return this->getPropertyByName<"data">();
}

boost::python::object &NestDevice::data()
{
	return this->getPropertyByName<"data">();
}
