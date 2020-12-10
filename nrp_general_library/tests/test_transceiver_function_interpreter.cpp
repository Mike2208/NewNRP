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

#include "tests/test_transceiver_function_interpreter.h"

TestSimpleTransceiverDevice::TestSimpleTransceiverDevice(boost::python::api::object fcn)
    : _fcn(fcn)
{}

TestOutputDevice::~TestOutputDevice() = default;

TestSimpleTransceiverDevice::~TestSimpleTransceiverDevice()
{	this->_tfInterpreterRegistry = nullptr;	}

TransceiverDeviceInterface::shared_ptr *TestSimpleTransceiverDevice::getTFInterpreterRegistry()
{	return this->_tfInterpreterRegistry;	}

const std::string &TestSimpleTransceiverDevice::linkedEngineName() const
{
	return this->_linkedEngine;
}

boost::python::object TestSimpleTransceiverDevice::runTf(python::tuple &args, python::dict &kwargs)
{	return this->_fcn(*args, **kwargs);	}

EngineInterface::device_identifiers_t TestSimpleTransceiverDevice::updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs) const
{	return std::move(deviceIDs);	}

TestInputDevice::~TestInputDevice() = default;

DeviceIdentifier TestOutputDevice::ID()
{
	return DeviceIdentifier("out", "engine", "type");
}

TestOutputDevice::TestOutputDevice()
    : DeviceInterface(TestOutputDevice::ID())
{}

DeviceIdentifier TestInputDevice::ID()
{
	return DeviceIdentifier("in", "engine", "type");
}

TestInputDevice::TestInputDevice()
    : DeviceInterface(TestInputDevice::ID())
{}

TestTransceiverDevice::~TestTransceiverDevice()
{	this->_tfInterpreterRegistry = nullptr;	}

const std::string &TestTransceiverDevice::linkedEngineName() const
{
	return this->_linkedEngine;
}
