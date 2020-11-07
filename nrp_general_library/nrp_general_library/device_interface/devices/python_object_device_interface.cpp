#include "nrp_general_library/device_interface/devices/python_object_device_interface.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_general_library/utils/python_error_handler.h"

namespace python = boost::python;

PythonObjectDeviceInterfaceConst::PyObjData::PyObjData(std::string _serializedData, python::object _data, python::object _jsonEncoder, python::object _jsonDecoder)
    : SerializedData(_serializedData),
      Data(_data),
      JsonEncoder(_jsonEncoder),
      JsonDecoder(_jsonDecoder)
{}

PythonObjectDeviceInterfaceConst::PyObjData::PyObjData(std::string _serializedData)
    : SerializedData(_serializedData)
{}

PythonObjectDeviceInterfaceConst::PyObjData::PyObjData(boost::python::object _data, boost::python::object _jsonEncoder, boost::python::object _jsonDecoder)
    : Data(_data),
      JsonEncoder(_jsonEncoder),
      JsonDecoder(_jsonDecoder)
{}

const std::string &PythonObjectDeviceInterfaceConst::PyObjData::serialize() const
{
	try
	{
		static python::object jsonDumps = python::import("json").attr("dumps");
		python::dict kwargs;
		kwargs["cls"] = this->JsonEncoder;
		const_cast<PyObjData*>(this)->SerializedData = python::extract<std::string>(python::str(jsonDumps(*python::make_tuple(this->Data), **kwargs)));		
		return this->SerializedData;
	}
	catch(python::error_already_set&)
	{
		throw NRPException::logCreate("Encountered unexpected python error during JSON serialization: " + handle_pyerror());
	}
}

python::object PythonObjectDeviceInterfaceConst::PyObjData::deserialize() const
{
	try
	{
		static python::object jsonLoads = python::import("json").attr("loads");
		python::dict kwargs;
		kwargs["cls"] = this->JsonDecoder;
		const_cast<PyObjData*>(this)->Data = jsonLoads(*python::make_tuple(this->SerializedData), **kwargs);
		return this->Data;
	}
	catch(python::error_already_set&)
	{
		throw NRPException::logCreate("Encountered unexpected python error during JSON deserialization: " + handle_pyerror());
	}
}

boost::python::object PythonObjectDeviceInterfaceConst::PyObjData::defaultEncoder()
{
	return python::import("json").attr("JSONEncoder");
}

boost::python::object PythonObjectDeviceInterfaceConst::PyObjData::defaultDecoder()
{
	return python::import("json").attr("JSONDecoder");
}

PythonObjectDeviceInterface::PythonObjectDeviceInterface(const DeviceIdentifier &id, const nlohmann::json &data)
    : DeviceInterface(id),
      PropertyTemplate(JSONPropertySerializer<PythonObjectDeviceInterface>::readProperties(data, PyObjData()))
{}

const PythonObjectDeviceInterfaceConst::PyObjData &PythonObjectDeviceInterface::data() const
{
	return this->template getPropertyByName<PythonObjectDeviceInterface::Object>();
}

PythonObjectDeviceInterfaceConst::PyObjData &PythonObjectDeviceInterface::data()
{
	return this->template getPropertyByName<PythonObjectDeviceInterface::Object>();
}

void PythonObjectDeviceInterface::setData(const PythonObjectDeviceInterfaceConst::PyObjData &data)
{	this->data() = data;	}

boost::python::api::object PythonObjectDeviceInterface::pyData() const
{	return this->data().Data;	}

void PythonObjectDeviceInterface::setPyData(boost::python::api::object data)
{	this->data().Data = data;	}

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PythonObjectDeviceInterfaceConst::PyObjData &property)
{
	return nlohmann::json::parse(property.serialize());
}

template<>
PythonObjectDeviceInterfaceConst::PyObjData JSONPropertySerializerMethods::deserializeSingleProperty<PythonObjectDeviceInterfaceConst::PyObjData>(const nlohmann::json &data, const std::string_view &name)
{
	auto retVal = PythonObjectDeviceInterfaceConst::PyObjData(data[name.data()].dump());
	retVal.deserialize();

	return retVal;
}

