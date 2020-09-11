#include "nrp_python_device_interface/engine_server/python_engine_json_device_controller.h"

#include <iostream>

PythonEngineJSONDeviceController<PythonObjectDeviceInterface>::PythonEngineJSONDeviceController(const DeviceIdentifier &devID, boost::python::object data)
    : EngineJSONDeviceController(devID),
      _deviceData(devID, data)
{}

nlohmann::json PythonEngineJSONDeviceController<PythonObjectDeviceInterface>::getDeviceInformation(const nlohmann::json::const_iterator &)
{
	return PropertySerializer<nlohmann::json, typename PythonObjectDeviceInterface::property_template_t>::serializeProperties(this->_deviceData, nlohmann::json());
}

nlohmann::json PythonEngineJSONDeviceController<PythonObjectDeviceInterface>::handleDeviceData(const nlohmann::json &data)
{
	// Update properties from data
	PropertySerializer<nlohmann::json, typename PythonObjectDeviceInterface::property_template_t>::updateProperties(this->_deviceData, data);

	return nlohmann::json();
}

boost::python::object &PythonEngineJSONDeviceController<PythonObjectDeviceInterface>::data()
{
	return this->_deviceData.data().Data;
}

boost::python::object PythonEngineJSONDeviceController<PythonObjectDeviceInterface>::data() const
{
	return this->_deviceData.data().Data;
}
