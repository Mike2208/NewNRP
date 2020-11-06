#include "nrp_nest_server_engine/devices/nest_server_device.h"
#include "nrp_nest_server_engine/config/nrp_nest_cmake_constants.h"

#include <iostream>

NestServerDevice::NestServerDevice(const DeviceIdentifier &devID, const nlohmann::json &json)
    : Device(devID, JSONPropertySerializer<NestServerDevice>::readProperties(json, "", boost::python::list(), boost::python::dict(), boost::python::dict()))
{
	this->getPropertyByName<"data">().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");
}

NestServerDevice::NestServerDevice(const DeviceIdentifier &devID, const std::string &cmd, const boost::python::list &args, const boost::python::dict &kwargs)
    : Device(devID, cmd, PyObjData(args), PyObjData(kwargs))
{}

NestServerDevice::NestServerDevice(const DeviceIdentifier &devID, const boost::python::object &data)
    : Device(devID, "", boost::python::list(), boost::python::dict(), data)
{
	this->getPropertyByName<"data">().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");
}

const std::string &NestServerDevice::cmd() const
{
	return this->getPropertyByName<"cmd">();
}

std::string &NestServerDevice::cmd()
{
	return this->getPropertyByName<"cmd">();
}

auto NestServerDevice::args() const -> const PyObjData&
{
	return this->getPropertyByName<"args">();
}

auto NestServerDevice::args() -> PyObjData&
{
	return this->getPropertyByName<"args">();
}

auto NestServerDevice::kwargs() const -> const PyObjData&
{
	return this->getPropertyByName<"kwargs">();
}

auto NestServerDevice::kwargs() -> PyObjData&
{
	return this->getPropertyByName<"kwargs">();
}

auto NestServerDevice::data() const -> const PyObjData&
{
	return this->getPropertyByName<"data">();
}

auto NestServerDevice::data() -> PyObjData&
{
	return this->getPropertyByName<"data">();
}
