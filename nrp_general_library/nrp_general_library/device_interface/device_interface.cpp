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

#include "nrp_general_library/device_interface/device.h"


DeviceIdentifier::DeviceIdentifier(const std::string &_name, const std::string &_engineName, const std::string &_type)
    : Name(_name), EngineName(_engineName), Type(_type)
{}

DeviceInterface::DeviceInterface(const std::string &name, const std::string &engineName, const std::string &type)
    : DeviceInterface(DeviceIdentifier(name, engineName, type))
{}

const std::string &DeviceInterface::name() const
{
	return this->_id.Name;
}

void DeviceInterface::setName(const std::string &name)
{
	this->_id.Name = name;
}

const std::string &DeviceInterface::type() const
{
	return this->_id.Type;
}

void DeviceInterface::setType(const std::string &type)
{
	this->_id.Type = type;
}

const std::string &DeviceInterface::engineName() const
{
	return this->_id.EngineName;
}

void DeviceInterface::setEngineName(const std::string &engineName)
{
	this->_id.EngineName = engineName;
}

const DeviceIdentifier &DeviceInterface::id() const
{
	return this->_id;
}

void DeviceInterface::setID(const DeviceIdentifier &id)
{
	this->_id = id;
}
