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
	return DeviceIdentifier("out", "type", "engine");
}

TestOutputDevice::TestOutputDevice()
    : DeviceInterface(TestOutputDevice::ID())
{}

DeviceIdentifier TestInputDevice::ID()
{
	return DeviceIdentifier("in", "type", "engine");
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
