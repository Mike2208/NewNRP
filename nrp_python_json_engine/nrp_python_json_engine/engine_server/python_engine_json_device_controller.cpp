#include "nrp_python_json_engine/engine_server/python_engine_json_device_controller.h"

#include <iostream>

PythonEngineJSONDeviceController<PyObjectDevice>::PythonEngineJSONDeviceController(DeviceIdentifier &&devID, boost::python::object data)
    : EngineDeviceController<nlohmann::json, PyObjectDevice>(std::move(devID)),
      _deviceData(devID, data)
{}

void PythonEngineJSONDeviceController<PyObjectDevice>::handleDeviceDataCallback(PyObjectDevice &&data)
{
	this->_deviceData = std::move(data);
}

const PyObjectDevice &PythonEngineJSONDeviceController<PyObjectDevice>::getDeviceInformationCallback()
{
	return this->_deviceData;
}

boost::python::object &PythonEngineJSONDeviceController<PyObjectDevice>::data()
{
	return this->_deviceData.data();
}

boost::python::object PythonEngineJSONDeviceController<PyObjectDevice>::data() const
{
	return this->_deviceData.data();
}
