//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_general_library/device_interface/devices/pyobject_device.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_general_library/utils/python_error_handler.h"

namespace python = boost::python;

PyObjectDeviceConst::PyObjData::PyObjData(const std::string &serializedData, boost::python::object _jsonEncoder, boost::python::object _jsonDecoder)
    : JsonEncoder(_jsonEncoder),
      JsonDecoder(_jsonDecoder)
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

