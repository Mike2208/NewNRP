#include "nrp_python_device_interface/python/py_engine_script.h"
#include "nrp_python_device_interface/engine_server/python_engine_json_device_controller.h"


PyEngineScript::~PyEngineScript()
{
	this->_pServer = nullptr;
}

void PyEngineScript::initialize()
{}

void PyEngineScript::shutdown()
{}

float PyEngineScript::simTime() const
{	return this->_time;	}

void PyEngineScript::registerDevice(std::string deviceName)
{
	//std::cout << "Registering device \"" + deviceName + "\"\n";
	assert(this->_pServer != nullptr);

	//std::cout << "Creating device controller for \"" + deviceName + "\"\n";
	PtrTemplates<PythonEngineJSONDeviceController<PythonObjectDeviceInterface>>::shared_ptr
	        newController(new PythonEngineJSONDeviceController<PythonObjectDeviceInterface>(DeviceIdentifier(deviceName, PythonObjectDeviceInterface::TypeName.data(), "")));

	//std::cout << "Adding device controller for \"" + deviceName + "\"\n";
	this->_deviceControllers.push_back(newController);
	this->_nameDeviceMap.emplace(deviceName, &(newController->data()));

	//std::cout << "Adding device \"" + deviceName + "\" to server\n";
	this->_pServer->registerDeviceNoLock(deviceName, newController.get());

	//std::cout << "Finished registering device \"" + deviceName + "\"\n";
}

boost::python::object &PyEngineScript::getDevice(const std::string &deviceName)
{
	auto devIt = this->_nameDeviceMap.find(deviceName);
	if(devIt == this->_nameDeviceMap.end())
	{
		const auto errMsg = "Could not find device with name \"" + deviceName + "\"";
		std::cerr << errMsg << std::endl;
		throw std::runtime_error(errMsg);
	}

	return *(devIt->second);
}

void PyEngineScript::setDevice(const std::string &deviceName, boost::python::object data)
{	this->getDevice(deviceName) = data;	}

void PyEngineScript::setPythonJSONServer(PythonJSONServer *pServer)
{
	this->_pServer = pServer;
}
