#include "nrp_python_json_engine/engine_server/python_engine_json_device_controller.h"

#include <iostream>

PythonEngineJSONDeviceController<PyObjectDevice>::PythonEngineJSONDeviceController(const DeviceIdentifier &devID, boost::python::object data)
    : EngineJSONDeviceController(devID),
      _deviceData(devID, data)
{}

nlohmann::json PythonEngineJSONDeviceController<PyObjectDevice>::getDeviceInformation(const nlohmann::json::const_iterator &)
{
	return PropertySerializer<nlohmann::json, typename PyObjectDevice::property_template_t>::serializeProperties(this->_deviceData, nlohmann::json());
}

nlohmann::json PythonEngineJSONDeviceController<PyObjectDevice>::handleDeviceData(const nlohmann::json &data)
{
	// Update properties from data
	PropertySerializer<nlohmann::json, typename PyObjectDevice::property_template_t>::updateProperties(this->_deviceData, data);

	return nlohmann::json();
}

boost::python::object &PythonEngineJSONDeviceController<PyObjectDevice>::data()
{
	return this->_deviceData.data().Data;
}

boost::python::object PythonEngineJSONDeviceController<PyObjectDevice>::data() const
{
	return this->_deviceData.data().Data;
}
