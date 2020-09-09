#include "nrp_general_library/transceiver_function/single_transceiver_device.h"

#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"

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
	const auto outputDeviceData = TransceiverDeviceInterface::TFInterpreter->outputDeviceData();

	bool foundDevID = false;
	auto engDevicesIt = outputDeviceData.find(this->_deviceID.EngineName);
	if(engDevicesIt != outputDeviceData.end())
	{
		for(const auto &curDevice : engDevicesIt->second)
		{
			if(curDevice->id() == this->_deviceID)
			{
				kwargs[this->_keyword] = curDevice;

				foundDevID = true;
				break;
			}
		}
	}

	if(!foundDevID)
		throw std::out_of_range("Couldn't find device with ID name \"" + this->_deviceID.Name + "\"");

	return TransceiverDeviceInterface::runTf(args, kwargs);
}
