#include "nrp_general_library/transceiver_function/single_transceiver_device.h"

#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

SingleTransceiverDevice::SingleTransceiverDevice(const std::string &keyword, const DeviceIdentifier &deviceID)
    : _keyword(keyword),
      _deviceID(deviceID)
{}

EngineInterface::device_identifiers_t SingleTransceiverDevice::getRequestedDeviceIDs() const
{
	return EngineInterface::device_identifiers_t({this->_deviceID});
}

boost::python::object SingleTransceiverDevice::runTf(boost::python::tuple &args, boost::python::dict &kwargs)
{
	const auto engineDevs = TransceiverDeviceInterface::TFInterpreter->engineDevices();

	bool foundDevID = false;
	auto engDevicesIt = engineDevs.find(this->_deviceID.EngineName);
	if(engDevicesIt != engineDevs.end())
	{
		for(const auto &curDevice : *(engDevicesIt->second))
		{
			if(curDevice->id().Name == this->_deviceID.Name)
			{
				kwargs[this->_keyword] = curDevice;

				foundDevID = true;
				break;
			}
		}
	}

	if(!foundDevID)
		throw NRPException::logCreate("Couldn't find device with ID name \"" + this->_deviceID.Name + "\"");

	return TransceiverDeviceInterface::runTf(args, kwargs);
}
