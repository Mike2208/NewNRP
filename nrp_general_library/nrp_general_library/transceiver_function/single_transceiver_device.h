#ifndef SINGLE_TRANSCEIVER_DEVICE_H
#define SINGLE_TRANSCEIVER_DEVICE_H

#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"

class SingleTransceiverDevice
        : public TransceiverDeviceInterface
{
	public:
		SingleTransceiverDevice(const std::string &keyword, const DeviceIdentifier &deviceID);
		virtual ~SingleTransceiverDevice() override = default;

		EngineInterface::device_identifiers_t getRequestedDeviceIDs() const override;

		boost::python::object runTf(boost::python::tuple &args, boost::python::dict &kwargs) override;

	private:

		std::string _keyword;
		DeviceIdentifier _deviceID;
};

#endif // SINGLE_TRANSCEIVER_DEVICE_H
