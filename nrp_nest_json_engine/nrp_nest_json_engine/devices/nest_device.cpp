#include "nrp_nest_json_engine/devices/nest_device.h"
#include "nrp_nest_json_engine/config/cmake_constants.h"


NestDevice::NestDevice(DeviceIdentifier &&devID, const boost::python::object &data)
    : Device(std::move(devID), data)
{	this->PyObjectDevice::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"];	}

NestDevice::NestDevice(const DeviceIdentifier &devID, const boost::python::object &data)
    : PyObjectDevice(devID, data)
{
	this->PyObjectDevice::data().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"];
}

const boost::python::object &NestDevice::data() const
{
	return this->getPropertyByName<"data">();
}

boost::python::object &NestDevice::data()
{
	return this->getPropertyByName<"data">();
}
