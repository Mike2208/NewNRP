#include <boost/python.hpp>

#include "nrp_general_library/config/cmake_constants.h"
#include "nrp_general_library/device_interface/python_device.h"
#include "nrp_example_engine/config/cmake_constants.h"
#include "nrp_example_engine/devices/example_device.h"


namespace python = boost::python;

BOOST_PYTHON_MODULE(PYTHON_MODULE_NAME)
{
	python::import(PYTHON_MODULE_NAME_STR);

	python_property_device_class<ExampleDevice>::create();
}
