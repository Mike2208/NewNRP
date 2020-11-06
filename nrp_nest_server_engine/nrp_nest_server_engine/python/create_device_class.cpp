#include "nrp_nest_server_engine/python/create_device_class.h"

CreateDeviceClass::CreateDeviceClass(boost::python::dict nest, boost::python::dict devMap)
    : _nest(nest),
      _devMap(devMap)
{}

boost::python::object CreateDeviceClass::pyCreateDevice(boost::python::tuple args, boost::python::dict kwargs)
{
	CreateDeviceClass &self = boost::python::extract<CreateDeviceClass&>(args[0]);
	return self.createAndRegisterDevice(args, kwargs);
}

boost::python::object CreateDeviceClass::pyRegisterDevice(boost::python::tuple args, boost::python::dict)
{
	CreateDeviceClass &self = boost::python::extract<CreateDeviceClass&>(args[0]);
	self.registerDevice(boost::python::str(args[1]), args[2]);

	return boost::python::object();
}

boost::python::api::object CreateDeviceClass::createAndRegisterDevice(boost::python::tuple args, boost::python::dict kwargs)
{
	// Call Nest's Create function with all arguments except for first one (that is the label)
	boost::python::object deviceID = this->_nest["Create"](*(args.slice(1, boost::python::len(args))), **kwargs);

	// Save created device's ID
	this->registerDevice(boost::python::str(args[0]), deviceID);

	return deviceID;
}

void CreateDeviceClass::registerDevice(boost::python::str devName, boost::python::api::object nodeCollection)
{
	this->_devMap[devName] = nodeCollection;
}

boost::python::dict CreateDeviceClass::pyDevMap()
{
	return this->devMap();
}

const boost::python::dict &CreateDeviceClass::devMap() const
{
	return this->_devMap;
}
