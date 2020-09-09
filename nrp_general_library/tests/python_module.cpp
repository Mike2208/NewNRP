#include "tests/test_env_cmake.h"
#include "tests/test_transceiver_function_interpreter.h"

#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(TEST_PYTHON_MODULE_NAME)
{
	class_<TestOutputDevice, bases<DeviceInterface> >("TestOutputDevice", init<>())
	        .def_readwrite("test_value", &TestOutputDevice::TestValue);

	python::register_ptr_to_python<std::shared_ptr<TestOutputDevice> >();
	python::register_ptr_to_python<std::shared_ptr<const TestOutputDevice> >();

	class_<TestInputDevice, bases<DeviceInterface> >("TestInputDevice", init<>())
	        .def_readwrite("test_value", &TestInputDevice::TestValue);

	python::register_ptr_to_python<std::shared_ptr<const TestInputDevice> >();
	python::register_ptr_to_python<std::shared_ptr<TestInputDevice> >();
}
