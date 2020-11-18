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
