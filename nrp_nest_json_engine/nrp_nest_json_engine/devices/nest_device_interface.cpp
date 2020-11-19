#include "nrp_nest_json_engine/devices/nest_device_interface.h"
#include "nrp_nest_json_engine/config/cmake_constants.h"

#include <iostream>

NestDeviceInterface::NestDeviceInterface(const DeviceIdentifier &devID, const nlohmann::json &json)
    : PyObjectDevice(devID, JSONPropertySerializer<NestDeviceInterface>::readProperties(json, boost::python::dict()))
{	this->PyObjectDevice::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"];	}

NestDeviceInterface::NestDeviceInterface(const DeviceIdentifier &devID, const boost::python::object &data)
    : PyObjectDevice(devID, data)
{
	this->PyObjectDevice::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"];
}

const boost::python::object &NestDeviceInterface::data() const
{
	return PyObjectDevice::data();
}

boost::python::object &NestDeviceInterface::data()
{
	return PyObjectDevice::data();
}
