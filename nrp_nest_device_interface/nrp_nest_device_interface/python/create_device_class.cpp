#include "nrp_nest_device_interface/python/create_device_class.h"

CreateDeviceClass::CreateDeviceClass(boost::python::dict nest, boost::python::dict devMap)
    : _nest(nest),
      _devMap(devMap)
{}

boost::python::object CreateDeviceClass::pyCreateDevice(boost::python::tuple args, boost::python::dict kwargs)
{
	CreateDeviceClass &self = boost::python::extract<CreateDeviceClass&>(args[0]);
	return self.createAndRegisterDevice(args, kwargs);
}

boost::python::api::object CreateDeviceClass::createAndRegisterDevice(boost::python::tuple args, boost::python::dict kwargs)
{
	// Call Nest's Create function with all arguments except for first one (that is the label)
	boost::python::object deviceID = this->_nest["Create"](*(args.slice(1, boost::python::len(args))), **kwargs);

	// Save created device's ID
	//boost::python::str label = static_cast<boost::python::str>(args[0]);
	this->_devMap[args[0]] = deviceID;

	return deviceID;
}

boost::python::dict CreateDeviceClass::pyDevMap()
{
	return this->devMap();
}

const boost::python::dict &CreateDeviceClass::devMap() const
{
	return this->_devMap;
}
