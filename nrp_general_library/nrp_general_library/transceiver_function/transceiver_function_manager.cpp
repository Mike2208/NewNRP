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

#include "nrp_general_library/transceiver_function/transceiver_function_manager.h"
#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

#include <iostream>

TransceiverFunctionManager::TransceiverFunctionSettings::TransceiverFunctionSettings(const TransceiverFunctionConfigSharedPtr &config)
    : TransceiverFunctionConfigSharedPtr(config)
{}

bool TransceiverFunctionManager::TransceiverFunctionSettings::operator<(const TransceiverFunctionManager::TransceiverFunctionSettings &rhs) const
{
	return (*this)->name() < rhs->name();
}

EngineInterface::device_identifiers_t TransceiverFunctionManager::updateRequestedDeviceIDs() const
{
	return this->_tfInterpreter.updateRequestedDeviceIDs();
}

void TransceiverFunctionManager::loadTF(const TransceiverFunctionConfigSharedPtr &tfConfig)
{
	auto storedConfigIterator = this->_tfSettings.find(tfConfig);
	auto loadedTF = this->_tfInterpreter.findTF(tfConfig->name());
	if(loadedTF != this->_tfInterpreter.loadedTFs().end() || storedConfigIterator != this->_tfSettings.end())
		throw NRPException::logCreate("TF with name " + tfConfig->name() + "already loaded");

	this->_tfSettings.insert(tfConfig);
	this->_tfInterpreter.loadTransceiverFunction(*tfConfig);
}

void TransceiverFunctionManager::updateTF(const TransceiverFunctionConfigSharedPtr &tfConfig)
{
	this->_tfInterpreter.updateTransceiverFunction(*tfConfig);
	this->_tfSettings.insert(tfConfig);
}

TransceiverFunctionManager::tf_results_t TransceiverFunctionManager::executeActiveTFs()
{
	tf_results_t tfResults;

	for(const auto &setting : this->_tfSettings)
	{
		if(setting->isActive())
		{
			// Get device outputs from transceiver function
			TransceiverFunctionInterpreter::device_list_t pyResult(this->_tfInterpreter.runSingleTransceiverFunction(setting->name()));
			TransceiverFunctionInterpreter::TFExecutionResult result(std::move(pyResult));

			// Extract pointers to retrieved devices
			result.extractDevices();

			tfResults.push_back(result);
		}
	}

	return tfResults;
}

TransceiverFunctionManager::tf_results_t TransceiverFunctionManager::executeActiveLinkedTFs(const std::string &engineName)
{
	tf_results_t tfResults;

	const auto linkedTFRange = this->_tfInterpreter.getLinkedTFs(engineName);

	for(auto curTFIt = linkedTFRange.first; curTFIt != linkedTFRange.second; ++curTFIt)
	{
		if(this->isActive(curTFIt->second.Name))
		{
			// Get device outputs from transceiver function
			TransceiverFunctionInterpreter::device_list_t pyResult(this->_tfInterpreter.runSingleTransceiverFunction(curTFIt->second));
			TransceiverFunctionInterpreter::TFExecutionResult result(std::move(pyResult));

			// Extract pointers to retrieved devices
			result.extractDevices();

			tfResults.push_back(result);
		}
	}

	return tfResults;
}

bool TransceiverFunctionManager::isActive(const std::string &tfName)
{
	for(const auto &curSetting : this->_tfSettings)
	{
		if(curSetting->name() == tfName)
			return curSetting->isActive();
	}

	return false;
}

TransceiverFunctionInterpreter &TransceiverFunctionManager::getInterpreter()
{	return this->_tfInterpreter;	}
