#include "nrp_nest_json_engine/devices/nest_device_interface.h"
#include "nrp_nest_json_engine/config/nrp_nest_cmake_constants.h"

#include <iostream>

NestDeviceInterface::NestDeviceInterface(const DeviceIdentifier &devID, const nlohmann::json &json)
    : PythonObjectDeviceInterface(devID, JSONPropertySerializer<NestDeviceInterface>::readProperties(json, boost::python::dict()))
{	this->PythonObjectDeviceInterface::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");	}

NestDeviceInterface::NestDeviceInterface(const DeviceIdentifier &devID, const boost::python::object &data)
    : PythonObjectDeviceInterface(devID, data)
{
	this->PythonObjectDeviceInterface::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"];
}

const boost::python::object &NestDeviceInterface::data() const
{
	return PythonObjectDeviceInterface::data().Data;
}

boost::python::object &NestDeviceInterface::data()
{
	return PythonObjectDeviceInterface::data().Data;
}
