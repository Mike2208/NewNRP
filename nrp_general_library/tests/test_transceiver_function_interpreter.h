/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"
#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"
#include "nrp_general_library/transceiver_function/single_transceiver_device.h"

#include <boost/python.hpp>
using namespace boost;

struct TestSimpleTransceiverDevice
        : public TransceiverDeviceInterface
{
	TestSimpleTransceiverDevice(python::object fcn);
	virtual ~TestSimpleTransceiverDevice() override;

	EngineInterface::device_identifiers_t getRequestedDeviceIDs() const override
	{	return EngineInterface::device_identifiers_t();	}

	TransceiverDeviceInterface::shared_ptr *getTFInterpreterRegistry() override;

	const std::string &linkedEngineName() const override;

	boost::python::object runTf(boost::python::tuple &args, boost::python::dict &kwargs) override;

	EngineInterface::device_identifiers_t updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs = EngineInterface::device_identifiers_t()) const override;

	TransceiverDeviceInterface::shared_ptr *_tfInterpreterRegistry = nullptr;
	python::object _fcn;

	std::string _linkedEngine = "engine";
};

struct TestOutputDevice
        : public DeviceInterface
{
	static DeviceIdentifier ID();

	TestOutputDevice();

	TestOutputDevice(const DeviceIdentifier &id)
	    : DeviceInterface(id)
	{}

	virtual ~TestOutputDevice() override;

	int TestValue = 0;
};

struct TestInputDevice
        : public DeviceInterface
{
	static DeviceIdentifier ID();

	TestInputDevice();

	TestInputDevice(const DeviceIdentifier &id)
	    : DeviceInterface(id)
	{}

	virtual ~TestInputDevice() override;

	std::string TestValue;
};


struct TestTransceiverDevice
        : public TransceiverDeviceInterface
{
	TestTransceiverDevice() = default;

	virtual ~TestTransceiverDevice() override;

	EngineInterface::device_identifiers_t updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&) const override
	{	return this->getRequestedDeviceIDs();	}

	EngineInterface::device_identifiers_t getRequestedDeviceIDs() const override
	{	return {TestOutputDevice::ID()};	}

	TransceiverDeviceInterface::shared_ptr *getTFInterpreterRegistry() override
	{	return this->_tfInterpreterRegistry;	}

	const std::string &linkedEngineName() const override;

	boost::python::object runTf(boost::python::tuple&, boost::python::dict&) override
	{
		const auto &outDev = TFInterpreter->engineDevices().begin()->second->front();
		TestInputDevice inDev(TestInputDevice::ID());
		inDev.TestValue = std::to_string(dynamic_cast<const TestOutputDevice*>(outDev.get())->TestValue);

		boost::python::list devices;
		devices.append(inDev);

		return std::move(devices);
	}

	TransceiverDeviceInterface::shared_ptr *_tfInterpreterRegistry = nullptr;
	std::string _linkedEngine = "engine";
};
