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
