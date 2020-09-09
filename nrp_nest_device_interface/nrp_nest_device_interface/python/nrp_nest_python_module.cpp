#include <boost/python.hpp>

#include "nrp_general_library/config_headers/nrp_cmake_constants.h"
#include "nrp_nest_device_interface/config/nrp_nest_cmake_constants.h"
#include "nrp_nest_device_interface/python/create_device_class.h"


namespace python = boost::python;

static CreateDeviceClass *pCreateDevice = nullptr;

python::object CreateDevice(python::tuple args, python::dict kwargs)
{
	assert(pCreateDevice != nullptr);
	return pCreateDevice->createAndRegisterDevice(args, kwargs);
}

python::dict GetDevMap()
{
	assert(pCreateDevice != nullptr);
	return pCreateDevice->pyDevMap();
}

BOOST_PYTHON_MODULE(NRP_NEST_PYTHON_MODULE)
{
	// Import General NRP Python Module
	python::import(PYTHON_MODULE_NAME_STR);

	// Setup CreateDevice and import Nest
	python::class_<CreateDeviceClass>("CreateDeviceClass", python::no_init)
	        .def("CreateDevice", python::raw_function(&CreateDeviceClass::pyCreateDevice))
	        .def("GetDevMap", &CreateDeviceClass::pyDevMap);

	python::dict devMap;
	python::dict nestDict(python::import("nest").attr("__dict__"));

	python::object pyCreateDevice(CreateDeviceClass(nestDict, devMap));
	python::scope().attr("__CreateDevice") = pyCreateDevice;

	CreateDeviceClass &createDevice = python::extract<CreateDeviceClass&>(pyCreateDevice);
	pCreateDevice = &createDevice;

	// Setup User Functions
	//python::def("CreateDevice", python::raw_function(CreateDevice));
	//python::def("GetDevMap",
	//            []() {	return GetDevMap(python::scope().attr("__CreateDevice"));	});

	// Setup Nest Create Device Function
	python::def("CreateDevice", python::raw_function(CreateDevice));
	python::def("GetDevMap", GetDevMap);
}
