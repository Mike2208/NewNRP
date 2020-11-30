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
