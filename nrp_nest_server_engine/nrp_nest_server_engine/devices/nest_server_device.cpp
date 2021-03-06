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

#include "nrp_nest_server_engine/devices/nest_server_device.h"

#include <iostream>

NestServerDevice::NestServerDevice(DeviceIdentifier &&devID, const std::string &data)
    : NestServerDevice(std::move(devID), property_template_t(PyObjData(data, boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder"))))
{}

NestServerDevice::NestServerDevice(DeviceIdentifier &&devID, property_template_t &&data)
    : Device(std::move(devID), std::move(data))
{
	this->getPropertyByName<"data">().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");
}
