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
