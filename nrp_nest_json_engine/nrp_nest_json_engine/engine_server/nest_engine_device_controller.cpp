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

#include "nrp_nest_json_engine/engine_server/nest_engine_device_controller.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_error_handler.h"

NestEngineJSONDeviceController<NestDevice>::NestEngineJSONDeviceController(DeviceIdentifier &&devID, boost::python::object nodeCollection, boost::python::dict nest)
    : EngineJSONDeviceController<NestDevice>(std::move(devID)),
	  _nest(nest),
	  _nodeCollection(nodeCollection),
      _deviceData(DeviceIdentifier(*this), getStatusFromNest(nest, nodeCollection))
{}

void NestEngineJSONDeviceController<NestDevice>::handleDeviceDataCallback(NestDevice &&data)
{
	// Update properties from data
	this->_deviceData.data() = data.data();

	// Update Nest status
	this->_nest["SetStatus"](this->_nodeCollection, this->_deviceData.data());
}

const NestDevice *NestEngineJSONDeviceController<NestDevice>::getDeviceInformationCallback()
{
	// Get device status from Nest
	try
	{
		this->_deviceData.data() = this->getStatusFromNest();
	}
	catch(boost::python::error_already_set &)
	{
		throw NRPException::logCreate("Failed to get Nest device status: " + handle_pyerror());
	}

	// Convert to JSON object
	return &(this->_deviceData);
}

void NestEngineJSONDeviceController<NestDevice>::setNestID(boost::python::dict nest, boost::python::object nodeCollection)
{
	this->_nest = nest;
	this->_nodeCollection = nodeCollection;
}

boost::python::dict NestEngineJSONDeviceController<NestDevice>::getStatusFromNest()
{	return getStatusFromNest(this->_nest, this->_nodeCollection);	}

boost::python::dict NestEngineJSONDeviceController<NestDevice>::getStatusFromNest(boost::python::dict &nest, const boost::python::object &nodeCollection)
{	return boost::python::dict(nest["GetStatus"](nodeCollection)[0]);	}
