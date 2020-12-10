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

#include "nrp_general_library/transceiver_function/transceiver_function.h"

TransceiverFunction::TransceiverFunction(std::string linkedEngine)
    : _linkedEngine(linkedEngine)
{}

const std::string &TransceiverFunction::linkedEngineName() const
{	return this->_linkedEngine;	}

TransceiverDeviceInterface::shared_ptr TransceiverFunction::pySetup(boost::python::object transceiverFunction)
{
	this->_function = transceiverFunction;

	auto tf = this->moveToSharedPtr<TransceiverFunction>();

	assert(tf->_tfInterpreterRegistryPtr == nullptr);
	if(tf->_tfInterpreterRegistryPtr == nullptr)
		tf->_tfInterpreterRegistryPtr = TransceiverFunction::TFInterpreter->registerNewTF(this->linkedEngineName(), tf);

	return tf;
}

boost::python::object TransceiverFunction::runTf(boost::python::tuple &args, boost::python::dict &kwargs)
{
	return this->_function(*args, **kwargs);
}

EngineInterface::device_identifiers_t TransceiverFunction::getRequestedDeviceIDs() const
{
	return EngineInterface::device_identifiers_t();
}

EngineInterface::device_identifiers_t TransceiverFunction::updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs) const
{
	return std::move(deviceIDs);
}

TransceiverDeviceInterface::shared_ptr *TransceiverFunction::getTFInterpreterRegistry()
{
	return this->_tfInterpreterRegistryPtr;
}
