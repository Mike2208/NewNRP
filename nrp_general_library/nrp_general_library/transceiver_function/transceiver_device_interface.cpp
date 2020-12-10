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

#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"

TransceiverFunctionInterpreter *TransceiverDeviceInterface::TFInterpreter = nullptr;

const std::string &TransceiverDeviceInterface::linkedEngineName() const
{
	return this->_function->linkedEngineName();
}

boost::python::object TransceiverDeviceInterface::runTf(boost::python::tuple &args, boost::python::dict &kwargs)
{
	return this->_function->runTf(args, kwargs);
}

EngineInterface::device_identifiers_t TransceiverDeviceInterface::updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs) const
{
	auto subDeviceIDs = this->_function->updateRequestedDeviceIDs(std::move(deviceIDs));
	auto newDeviceIDs = this->getRequestedDeviceIDs();

	subDeviceIDs.insert(newDeviceIDs.begin(), newDeviceIDs.end());

	return subDeviceIDs;
}

EngineInterface::device_identifiers_t TransceiverDeviceInterface::getRequestedDeviceIDs() const
{
	return EngineInterface::device_identifiers_t();
}

void TransceiverDeviceInterface::setTFInterpreter(TransceiverFunctionInterpreter *interpreter)
{
	TransceiverDeviceInterface::TFInterpreter = interpreter;
}

TransceiverDeviceInterface::shared_ptr *TransceiverDeviceInterface::getTFInterpreterRegistry()
{
	return this->_function->getTFInterpreterRegistry();
}
