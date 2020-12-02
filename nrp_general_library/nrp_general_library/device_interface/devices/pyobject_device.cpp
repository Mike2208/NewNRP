#include "nrp_general_library/device_interface/devices/pyobject_device.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_general_library/utils/python_error_handler.h"

namespace python = boost::python;

PyObjectDeviceConst::PyObjData::PyObjData(const std::string &serializedData)
{
	this->deserialize(serializedData);
}

PyObjectDeviceConst::PyObjData::PyObjData(boost::python::object _data, boost::python::object _jsonEncoder, boost::python::object _jsonDecoder)
    : boost::python::object(_data),
      JsonEncoder(_jsonEncoder),
      JsonDecoder(_jsonDecoder)
{}

std::string PyObjectDeviceConst::PyObjData::serialize() const
{
	try
	{
		python::object jsonDumps = python::import("json").attr("dumps");
		python::dict kwargs;
		kwargs["cls"] = this->JsonEncoder;
		return python::extract<std::string>(python::str(jsonDumps(*python::make_tuple(reinterpret_cast<const boost::python::object&>(*this)), **kwargs)));
	}
	catch(python::error_already_set&)
	{
		throw NRPException::logCreate("Encountered unexpected python error during JSON serialization: " + handle_pyerror());
	}
}

python::object PyObjectDeviceConst::PyObjData::deserialize(const std::string &serializedData)
{
	try
	{
		python::object jsonLoads = python::import("json").attr("loads");
		python::dict kwargs;
		kwargs["cls"] = this->JsonDecoder;
		reinterpret_cast<boost::python::object&>(*this) = jsonLoads(*python::make_tuple(serializedData), **kwargs);
		return *this;
	}
	catch(python::error_already_set&)
	{
		throw NRPException::logCreate("Encountered unexpected python error during JSON deserialization: " + handle_pyerror());
	}
}

boost::python::object PyObjectDeviceConst::PyObjData::defaultEncoder()
{
	return python::import("json").attr("JSONEncoder");
}

boost::python::object PyObjectDeviceConst::PyObjData::defaultDecoder()
{
	return python::import("json").attr("JSONDecoder");
}

PyObjectDevice::PyObjectDevice(const DeviceIdentifier &id, const nlohmann::json &data)
    : Device(id, JSONPropertySerializer<PyObjectDevice>::readProperties(data, PyObjData()))
{}

const PyObjectDeviceConst::PyObjData &PyObjectDevice::data() const
{
	return this->template getPropertyByName<PyObjectDevice::Object>();
}

PyObjectDeviceConst::PyObjData &PyObjectDevice::data()
{
	return this->template getPropertyByName<PyObjectDevice::Object>();
}

void PyObjectDevice::setData(const PyObjectDeviceConst::PyObjData &data)
{	this->data() = data;	}

boost::python::object PyObjectDevice::pyData() const
{	return this->data();	}

void PyObjectDevice::setPyData(boost::python::object data)
{	this->data() = data;	}

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PyObjectDeviceConst::PyObjData &property)
{
	return nlohmann::json::parse(property.serialize());
}

template<>
PyObjectDeviceConst::PyObjData JSONPropertySerializerMethods::deserializeSingleProperty<PyObjectDeviceConst::PyObjData>(const nlohmann::json &data, const std::string_view &name)
{
	return PyObjectDeviceConst::PyObjData(data[name.data()].dump());
}

