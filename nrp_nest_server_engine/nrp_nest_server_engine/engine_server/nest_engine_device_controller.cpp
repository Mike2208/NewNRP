#include "nrp_nest_server_engine/engine_server/nest_engine_device_controller.h"

NestEngineJSONDeviceController<NestServerDevice>::NestEngineJSONDeviceController(const DeviceIdentifier &devID, boost::python::object nodeCollection, boost::python::dict nest)
	: EngineJSONDeviceController(devID),
	  _nest(nest),
	  _nodeCollection(nodeCollection),
      _deviceData(devID, getStatusFromNest(nest, nodeCollection))
{}

nlohmann::json NestEngineJSONDeviceController<NestServerDevice>::getDeviceInformation(const nlohmann::json::const_iterator &)
{
	// Get device status from Nest
	boost::python::dict status;
	try
	{
		status = this->getStatusFromNest();
	}
	catch(boost::python::error_already_set &)
	{
		PyErr_Print();
		PyErr_Clear();

		throw;
	}

	// Read properties from dict object
	this->_deviceData.data().Data = status;

	// Convert to JSON object
	return PropertySerializer<nlohmann::json, typename NestServerDevice::property_template_t>::serializeProperties(this->_deviceData);
}

nlohmann::json NestEngineJSONDeviceController<NestServerDevice>::handleDeviceData(const nlohmann::json &data)
{
	// Update properties from data
	PropertySerializer<nlohmann::json, typename NestServerDevice::property_template_t>::updateProperties(this->_deviceData, data);

	// Convert to python dict object
	//this->_deviceData.updateProperties(this->_deviceData, data);

	// Update Nest status
	this->_nest["SetStatus"](this->_nodeCollection, this->_deviceData.data().Data);

	return nlohmann::json();
}

void NestEngineJSONDeviceController<NestServerDevice>::setNestID(boost::python::dict nest, boost::python::object nodeCollection)
{
	this->_nest = nest;
	this->_nodeCollection = nodeCollection;
}

boost::python::dict NestEngineJSONDeviceController<NestServerDevice>::getStatusFromNest()
{	return getStatusFromNest(this->_nest, this->_nodeCollection);	}

boost::python::dict NestEngineJSONDeviceController<NestServerDevice>::getStatusFromNest(boost::python::dict &nest, const boost::python::object &nodeCollection)
{	return boost::python::dict(nest["GetStatus"](nodeCollection)[0]);	}
