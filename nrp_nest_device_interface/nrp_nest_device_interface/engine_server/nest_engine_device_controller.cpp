#include "nrp_nest_device_interface/engine_server/nest_engine_device_controller.h"

NestEngineJSONDeviceController<NestJSONDeviceInterface>::NestEngineJSONDeviceController(const DeviceIdentifier &devID, boost::python::object nodeCollection, boost::python::dict nest)
	: EngineGrpcDeviceController(devID),
	  _nest(nest),
	  _nodeCollection(nodeCollection),
      _deviceData(devID, getStatusFromNest(nest, nodeCollection))
{}

void NestEngineJSONDeviceController<NestJSONDeviceInterface>::getData(EngineGrpc::GetDeviceMessage * reply)
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
	this->_deviceData.NestJSONDeviceInterface::data() = status;

	// Return data as json embedded in protobuf
	reply->mutable_nest()->set_json(PropertySerializer<nlohmann::json, typename NestJSONDeviceInterface::property_template_t>::serializeProperties(this->_deviceData));
}

void NestEngineJSONDeviceController<NestJSONDeviceInterface>::setData(const google::protobuf::Message & data)
{
	auto newData = static_cast<const EngineGrpc::Nest &>(data);

	// Update properties from data
	PropertySerializer<nlohmann::json, typename NestJSONDeviceInterface::property_template_t>::updateProperties(this->_deviceData, nlohmann::json::parse(newData.json()));

	// Convert to python dict object
	//this->_deviceData.updateProperties(this->_deviceData, data);

	// Update Nest status
	this->_nest["SetStatus"](this->_nodeCollection, this->_deviceData.data());
}

void NestEngineJSONDeviceController<NestJSONDeviceInterface>::setNestID(boost::python::dict nest, boost::python::object nodeCollection)
{
	this->_nest = nest;
	this->_nodeCollection = nodeCollection;
}

boost::python::dict NestEngineJSONDeviceController<NestJSONDeviceInterface>::getStatusFromNest()
{	return getStatusFromNest(this->_nest, this->_nodeCollection);	}

boost::python::dict NestEngineJSONDeviceController<NestJSONDeviceInterface>::getStatusFromNest(boost::python::dict &nest, const boost::python::object &nodeCollection)
{	return boost::python::dict(nest["GetStatus"](nodeCollection)[0]);	}
