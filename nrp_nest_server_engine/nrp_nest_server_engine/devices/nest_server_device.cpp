#include "nrp_nest_server_engine/devices/nest_server_device.h"

#include <iostream>

NestServerDevice::NestServerDevice(DeviceIdentifier &&devID, const std::string &data)
    : NestServerDevice(std::move(devID), property_template_t(PyObjData(data, boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder"))))
{}

NestServerDevice::NestServerDevice(DeviceIdentifier &&devID, property_template_t &&data)
    : Device(std::move(devID), std::move(data))
{
	this->getPropertyByName<"data">().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");
}
