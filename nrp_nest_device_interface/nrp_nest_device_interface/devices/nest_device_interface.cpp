#include "nrp_nest_device_interface/devices/nest_device_interface.h"
#include "nrp_nest_device_interface/config/nrp_nest_cmake_constants.h"

#include <iostream>

NestJSONDeviceInterface::NestJSONDeviceInterface(const DeviceIdentifier &devID, const nlohmann::json &json)
    : PythonObjectDeviceInterface(devID, JSONPropertySerializer<NestJSONDeviceInterface>::readProperties(json, boost::python::dict()))
{	this->PythonObjectDeviceInterface::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");	}

NestJSONDeviceInterface::NestJSONDeviceInterface(const DeviceIdentifier &devID, const boost::python::dict &data)
    : PythonObjectDeviceInterface(devID, data)
{
	this->PythonObjectDeviceInterface::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"];
}

boost::python::dict NestJSONDeviceInterface::data() const
{
	return boost::python::dict(PythonObjectDeviceInterface::data().Data);
}