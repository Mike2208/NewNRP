#include "nrp_gazebo_device_interface/devices/json_physics_link.h"


JSONPhysicsLink::JSONPhysicsLink(const std::string &name)
    : JSONPhysicsLink(DeviceIdentifier(name, TypeName.data(), ""))
{}

JSONPhysicsLink::JSONPhysicsLink(const DeviceIdentifier &id)
    : JSONDeviceInterface(id, DefPosition, DefRotation,
                          DefLinearVelocity, DefAngularVelocity)
{}

JSONPhysicsLink::JSONPhysicsLink(const DeviceIdentifier &id, const nlohmann::json &data)
    : JSONDeviceInterface(id, data, DefPosition, DefRotation,
                          DefLinearVelocity, DefAngularVelocity)
{}

const JSONPhysicsLink::vec3_t &JSONPhysicsLink::position() const
{
	return this->getPropertyByName<Position, vec3_t>();
}

void JSONPhysicsLink::setPosition(const JSONPhysicsLink::vec3_t &position)
{
	this->getPropertyByName<Position, vec3_t>() = position;
}

const JSONPhysicsLink::quat_t &JSONPhysicsLink::rotation() const
{
	return this->getPropertyByName<Rotation, quat_t>();
}

void JSONPhysicsLink::setRotation(const JSONPhysicsLink::quat_t &rotation)
{
	this->getPropertyByName<Rotation, quat_t>() = rotation;
}

const JSONPhysicsLink::vec3_t &JSONPhysicsLink::linVel() const
{
	return this->getPropertyByName<LinearVelocity, vec3_t>();
}

void JSONPhysicsLink::setLinVel(const JSONPhysicsLink::vec3_t &linVel)
{
	this->getPropertyByName<LinearVelocity, vec3_t>() = linVel;
}

const JSONPhysicsLink::vec3_t &JSONPhysicsLink::angVel() const
{
	return this->getPropertyByName<AngularVelocity, vec3_t>();
}

void JSONPhysicsLink::setAngVel(const JSONPhysicsLink::vec3_t &angVel)
{
	this->getPropertyByName<AngularVelocity, vec3_t>() = angVel;
}
