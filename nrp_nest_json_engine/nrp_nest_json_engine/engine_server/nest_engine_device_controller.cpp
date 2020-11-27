#include "nrp_nest_json_engine/engine_server/nest_engine_device_controller.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_error_handler.h"

NestEngineJSONDeviceController<NestDevice>::NestEngineJSONDeviceController(const DeviceIdentifier &devID, boost::python::object nodeCollection, boost::python::dict nest)
	: EngineJSONDeviceController(devID),
	  _nest(nest),
	  _nodeCollection(nodeCollection),
      _deviceData(devID, getStatusFromNest(nest, nodeCollection))
{}

nlohmann::json NestEngineJSONDeviceController<NestDevice>::getDeviceInformation(const nlohmann::json::const_iterator &)
{
	// Get device status from Nest
	boost::python::dict status;
	try
	{
		status = this->getStatusFromNest();
	}
	catch(boost::python::error_already_set &)
	{
		throw NRPException::logCreate("Failed to get Nest device status: " + handle_pyerror());
	}

	// Read properties from dict object
	this->_deviceData.NestDevice::data() = status;

	// Convert to JSON object
	return PropertySerializer<nlohmann::json, typename NestDevice::property_template_t>::serializeProperties(this->_deviceData);
}

nlohmann::json NestEngineJSONDeviceController<NestDevice>::handleDeviceData(const nlohmann::json &data)
{
	// Update properties from data
	PropertySerializer<nlohmann::json, typename NestDevice::property_template_t>::updateProperties(this->_deviceData, data);

	// Convert to python dict object
	//this->_deviceData.updateProperties(this->_deviceData, data);

	// Update Nest status
	this->_nest["SetStatus"](this->_nodeCollection, this->_deviceData.data());

	return nlohmann::json();
}

void NestEngineJSONDeviceController<NestDevice>::setNestID(boost::python::dict nest, boost::python::object nodeCollection)
{
	this->_nest = nest;
	this->_nodeCollection = nodeCollection;
}

boost::python::dict NestEngineJSONDeviceController<NestDevice>::getStatusFromNest()
{	return getStatusFromNest(this->_nest, this->_nodeCollection);	}

boost::python::dict NestEngineJSONDeviceController<NestDevice>::getStatusFromNest(boost::python::dict &nest, const boost::python::object &nodeCollection)
{	return boost::python::dict(nest["GetStatus"](nodeCollection)[0]);	}
