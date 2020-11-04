#include "nrp_general_library/device_interface/device_interface.h"


DeviceIdentifier::DeviceIdentifier(const std::string &_name, const std::string &_type, const std::string &_engineName)
    : Name(_name), Type(_type), EngineName(_engineName)
{}

bool DeviceIdentifier::operator==(const DeviceIdentifier &other) const
{
	return this->Name.compare(other.Name) == 0
	        && this->Type.compare(other.Type) == 0
	        && this->EngineName.compare(other.EngineName) == 0;
}

bool DeviceIdentifier::operator<(const DeviceIdentifier &other) const
{
	const auto nameCmp = this->Name.compare(other.Name);
	if(nameCmp < 0)
		return true;
	else if(nameCmp == 0)
	{
		const auto typeCmp = this->Type.compare(other.Type);
		if(typeCmp < 0)
			return true;
		else if(typeCmp == 0)
			return this->EngineName.compare(other.EngineName) < 0;
	}

	return false;
}

DeviceInterface::DeviceInterface(const DeviceIdentifier &id)
    : _id(id)
{}

DeviceInterface::DeviceInterface(const std::string &name, const std::string &type, const std::string &engineName)
    : _id(name, type, engineName)
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

void DeviceInterface::setId(const DeviceIdentifier &id)
{
	this->_id = id;
}

void DeviceInterface::serialize(EngineGrpc::SetDeviceMessage * ) const
{

}

void DeviceInterface::deserialize(const EngineGrpc::GetDeviceMessage & )
{

}