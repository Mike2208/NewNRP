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

#include <gtest/gtest.h>

#include "nrp_general_library/config/cmake_constants.h"
#include "tests/test_transceiver_function_interpreter.h"
#include "tests/test_env_cmake.h"

using namespace boost;

void appendPythonPath(const std::string &path)
{
	boost::python::handle pathH(boost::python::borrowed(PySys_GetObject("path")));
	boost::python::list paths(pathH);
	paths.append(path);

	PySys_SetObject("path", paths.ptr());
}

TEST(TransceiverFunctionInterpreterTest, TestSimplePythonFcn)
{
	Py_Initialize();
	python::object main(python::import("__main__"));
	python::dict globals(main.attr("__dict__"));
	try
	{
		// Append simple_function path to search
		appendPythonPath(TEST_SIMPLE_TRANSCEIVER_FCN_MODULE_PATH);

		// Load simple function
		python::object simpleFcn(python::import("simple_fcn"));
		globals.update(simpleFcn.attr("__dict__"));
		globals["simple_fcn"]();
	}
	catch(boost::python::error_already_set &)
	{
		PyErr_Print();
		PyErr_Clear();

		std::cout.flush();

		throw std::invalid_argument("Python Exception");
	}

	TransceiverFunctionInterpreterSharedPtr interpreter(new TransceiverFunctionInterpreter(globals));

	TransceiverDeviceInterface::setTFInterpreter(interpreter.get());

	// Load and execute simple python function
	const std::string tfName = "testTF";
	TestSimpleTransceiverDevice::shared_ptr tfDevice(new TestSimpleTransceiverDevice(globals["simple_fcn"]));
	interpreter->loadTransceiverFunction(tfName, tfDevice);

	// Test execution result
	boost::python::list res(interpreter->runSingleTransceiverFunction(tfName));
	ASSERT_EQ((int)boost::python::extract<int>(res[0]), 3);

	// Test invalid TF name
	ASSERT_THROW(interpreter->runSingleTransceiverFunction("invalidTFName"), NRPExceptionNonRecoverable);

	TransceiverDeviceInterface::setTFInterpreter(nullptr);
}

TEST(TransceiverFunctionInterpreterTest, TestTransceiverFcnDevices)
{
	Py_Initialize();
	python::object main(python::import("__main__"));
	python::object nrpModule(python::import(PYTHON_MODULE_NAME_STR));

	appendPythonPath(TEST_PYTHON_MODULE_PATH);
	python::object testModule(python::import(TEST_PYTHON_MODULE_NAME_STR));

	python::dict globals(main.attr("__dict__"));
	globals.update(nrpModule.attr("__dict__"));
	globals.update(testModule.attr("__dict__"));

	TransceiverFunctionInterpreterSharedPtr interpreter(new TransceiverFunctionInterpreter(globals));

	TransceiverDeviceInterface::setTFInterpreter(interpreter.get());

	std::shared_ptr<TestOutputDevice> dev(new TestOutputDevice(TestOutputDevice::ID()));
	EngineInterface::device_outputs_t devs({dev});
	interpreter->setEngineDevices({{dev->engineName(), &devs}});

	// Load and execute simple python function
	const std::string tfName = "testTF";
	TestTransceiverDevice::shared_ptr tfDevice(new TestTransceiverDevice());
	interpreter->loadTransceiverFunction(tfName, tfDevice);

	const auto &reqIDs = interpreter->updateRequestedDeviceIDs();
	ASSERT_EQ(reqIDs.size(), 1);
	ASSERT_EQ(*(reqIDs.begin()), TestOutputDevice::ID());

	// Test execution result
	boost::python::list res;
	res = static_cast<boost::python::list>(interpreter->runSingleTransceiverFunction(tfName));

	ASSERT_EQ(boost::python::len(res), 1);

	const TestInputDevice &inDevice = boost::python::extract<TestInputDevice>(res[0]);
	ASSERT_EQ(inDevice.id(), TestInputDevice::ID());
	ASSERT_EQ(dev->TestValue, std::stoi(inDevice.TestValue));

	TransceiverDeviceInterface::setTFInterpreter(nullptr);
}

TEST(TransceiverFunctionInterpreterTest, TestTransceiverFunction)
{
	Py_Initialize();
	python::object main(python::import("__main__"));
	python::object nrpModule(python::import(PYTHON_MODULE_NAME_STR));

	appendPythonPath(TEST_PYTHON_MODULE_PATH);
	python::object testModule(python::import(TEST_PYTHON_MODULE_NAME_STR));

	python::dict globals(main.attr("__dict__"));
	globals.update(nrpModule.attr("__dict__"));
	globals.update(testModule.attr("__dict__"));
//	globals.update(transceiverFcnModule.attr("__dict__"));

	TransceiverFunctionInterpreterSharedPtr interpreter(new TransceiverFunctionInterpreter(globals));

	TransceiverDeviceInterface::setTFInterpreter(interpreter.get());

	const std::string tfName = "testTF";

	TransceiverFunctionConfig tfCfg;
	tfCfg.setName(tfName);
	tfCfg.setFileName(TEST_TRANSCEIVER_FCN_FILE_NAME);
	tfCfg.setIsActive(true);

	std::shared_ptr<TestOutputDevice> dev(new TestOutputDevice(TestOutputDevice::ID()));
	dev->TestValue = 4;
	EngineInterface::device_outputs_t devs({dev});
	interpreter->setEngineDevices({{dev->engineName(), &devs}});

	// Load and execute simple python function
	interpreter->loadTransceiverFunction(tfCfg);

	const auto &reqIDs = interpreter->updateRequestedDeviceIDs();
	ASSERT_EQ(reqIDs.size(), 1);
	ASSERT_EQ(*(reqIDs.begin()), TestOutputDevice::ID());

	// Test execution result
	boost::python::list res(interpreter->runSingleTransceiverFunction(tfName));
	ASSERT_EQ(boost::python::len(res), 1);

	const TestInputDevice &inDevice = boost::python::extract<TestInputDevice>(res[0]);
	ASSERT_EQ(inDevice.id(), TestInputDevice::ID());
	ASSERT_EQ(dev->TestValue, std::stoi(inDevice.TestValue));

	TransceiverDeviceInterface::setTFInterpreter(nullptr);
}

