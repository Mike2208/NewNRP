//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

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
