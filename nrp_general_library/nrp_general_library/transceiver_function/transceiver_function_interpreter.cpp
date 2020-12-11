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

#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_error_handler.h"

#include <iostream>

TransceiverFunctionInterpreter::TransceiverFunctionData::TransceiverFunctionData(const std::string &_name, const TransceiverDeviceInterface::shared_ptr &_transceiverFunction, const EngineInterface::device_identifiers_t &_deviceIDs, const boost::python::object &_localVariables)
    : Name(_name),
      TransceiverFunction(_transceiverFunction),
      DeviceIDs(_deviceIDs),
      LocalVariables(_localVariables)
{}

void TransceiverFunctionInterpreter::TFExecutionResult::extractDevices()
{
	// Extract pointers to retrieved devices
	const auto devListLength = boost::python::len(this->DeviceList);
	this->Devices.reserve(devListLength);
	for(unsigned int i = 0; i < devListLength; ++i)
	{
		this->Devices.push_back(boost::python::extract<DeviceInterface*>(this->DeviceList[i]));
	}
}

TransceiverFunctionInterpreter::TFExecutionResult::TFExecutionResult(device_list_t &&_deviceList)
    : DeviceList(_deviceList)
{}

TransceiverFunctionInterpreter::TransceiverFunctionInterpreter()
    : TransceiverFunctionInterpreter(static_cast<boost::python::dict>(boost::python::import("__main__").attr("__dict__")))
{}

TransceiverFunctionInterpreter::TransceiverFunctionInterpreter(const boost::python::dict &tfGlobals)
    : _globalDict(tfGlobals)
{}

TransceiverFunctionInterpreter::transceiver_function_datas_t::const_iterator TransceiverFunctionInterpreter::findTF(const std::string &name) const
{
	for(auto curTFIterator = this->_transceiverFunctions.begin(); curTFIterator != this->_transceiverFunctions.end(); ++curTFIterator)
	{
		if(curTFIterator->second.Name.compare(name) == 0)
			return curTFIterator;
	}

	return this->_transceiverFunctions.end();
}

const TransceiverFunctionInterpreter::transceiver_function_datas_t &TransceiverFunctionInterpreter::loadedTFs() const
{
	return this->_transceiverFunctions;
}

EngineInterface::device_identifiers_t TransceiverFunctionInterpreter::updateRequestedDeviceIDs() const
{
	EngineInterface::device_identifiers_t devIDs;
	for(const auto &curData : this->_transceiverFunctions)
	{
		auto newDevIDs = curData.second.TransceiverFunction->updateRequestedDeviceIDs();
		devIDs.insert(newDevIDs.begin(), newDevIDs.end());
	}

	return devIDs;
}

void TransceiverFunctionInterpreter::setEngineDevices(TransceiverFunctionInterpreter::engines_devices_t &&engineDevices)
{
	this->_engineDevices = std::move(engineDevices);
}

boost::python::object TransceiverFunctionInterpreter::runSingleTransceiverFunction(const std::string &tfName)
{
	// Find associated TF
	auto tfDataIterator = this->findTF(tfName);

	// If TF doesn't exist yet, throw error
	if(tfDataIterator == this->_transceiverFunctions.end())
		throw NRPException::logCreate("TF with name " + tfName + "not loaded");

	return this->runSingleTransceiverFunction(tfDataIterator->second);
}

boost::python::object TransceiverFunctionInterpreter::runSingleTransceiverFunction(const TransceiverFunctionInterpreter::TransceiverFunctionData &tfData)
{
	try
	{
		boost::python::tuple args;
		boost::python::dict kwargs;

		boost::python::object retVal = tfData.TransceiverFunction->runTf(args, kwargs);

		// Make sure that tf returns a list. If not, return an empty list
		if(!boost::python::extract<boost::python::list>(retVal).check())
			return boost::python::list();
		else
			return retVal;
	}
	catch(boost::python::error_already_set &)
	{
		throw NRPException::logCreate("Python error occured during execution of TF \"" + tfData.Name + "\": " + handle_pyerror());
	}
}

std::pair<TransceiverFunctionInterpreter::transceiver_function_datas_t::iterator, TransceiverFunctionInterpreter::transceiver_function_datas_t::iterator>
TransceiverFunctionInterpreter::getLinkedTFs(const std::string &engineName)
{
	return this->_transceiverFunctions.equal_range(engineName);
}

TransceiverFunctionInterpreter::transceiver_function_datas_t::iterator TransceiverFunctionInterpreter::loadTransceiverFunction(const TransceiverFunctionConfig &transceiverFunction)
{
	// Make sure no previously loaded TFs have not been handled
	assert(this->_newTFIt == this->_transceiverFunctions.end());

	// Create dict for new module
	boost::python::dict localVars;

	// Load TF code
	try
	{
		boost::python::exec_file(transceiverFunction.fileName().data(), this->_globalDict, this->_globalDict);
	}
	catch(boost::python::error_already_set &)
	{
		const auto err = NRPException::logCreate("Loading of TransceiverFunction file \"" + transceiverFunction.fileName() + "\" failed: " + handle_pyerror());

		if(this->_newTFIt != this->_transceiverFunctions.end())
		{
			this->_transceiverFunctions.erase(this->_newTFIt);
			this->_newTFIt = this->_transceiverFunctions.end();
		}

		throw err;
	}

	// Check that load resulted in a TF
	if(this->_newTFIt == this->_transceiverFunctions.end())
		throw NRPException::logCreate("No TF found for " + transceiverFunction.name());

	// Update transfer function params
	this->_newTFIt->second.DeviceIDs      = this->_newTFIt->second.TransceiverFunction->updateRequestedDeviceIDs(EngineInterface::device_identifiers_t());
	this->_newTFIt->second.LocalVariables = localVars;
	this->_newTFIt->second.Name           = transceiverFunction.name();

	const auto retVal = this->_newTFIt;
	this->_newTFIt = this->_transceiverFunctions.end();

	return retVal;
}

TransceiverFunctionInterpreter::transceiver_function_datas_t::iterator TransceiverFunctionInterpreter::loadTransceiverFunction(const std::string &tfName, const TransceiverDeviceInterfaceSharedPtr &transceiverFunction, boost::python::object &&localVars)
{
	return this->_transceiverFunctions.emplace(transceiverFunction->linkedEngineName(), TransceiverFunctionData(tfName, transceiverFunction, transceiverFunction->updateRequestedDeviceIDs(), std::move(localVars))).first;
}

TransceiverFunctionInterpreter::transceiver_function_datas_t::iterator TransceiverFunctionInterpreter::updateTransceiverFunction(const TransceiverFunctionConfig &transceiverFunction)
{
	// Erase existing TF if found
	auto tfIterator = this->findTF(transceiverFunction.name());
	if(tfIterator != this->_transceiverFunctions.end())
		this->_transceiverFunctions.erase(tfIterator);

	// Create new one
	return this->loadTransceiverFunction(transceiverFunction);
}

TransceiverDeviceInterface::shared_ptr *TransceiverFunctionInterpreter::registerNewTF(const std::string &linkedEngine, const TransceiverDeviceInterface::shared_ptr &transceiverFunction)
{
	// Check that no previous TF has not been processed
	assert(this->_newTFIt == this->_transceiverFunctions.end());

	auto newTFIt = this->_transceiverFunctions.emplace(linkedEngine, TransceiverFunctionData("", transceiverFunction, {}, boost::python::dict())).first;
	this->_newTFIt = newTFIt;

	return &(newTFIt->second.TransceiverFunction);
}
