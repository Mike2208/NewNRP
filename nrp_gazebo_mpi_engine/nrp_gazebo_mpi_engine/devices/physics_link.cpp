#include "nrp_gazebo_mpi_engine/devices/physics_link.h"


PhysicsLink::PhysicsLink(const std::string &name)
    : PhysicsLink(DeviceIdentifier(name, TypeName.data(), ""))
{}

PhysicsLink::PhysicsLink(const DeviceIdentifier &id)
    : DeviceInterface(id),
      PropertyTemplate(DefPosition, DefRotation,
                       DefLinearVelocity, DefAngularVelocity)
{}

PhysicsLink::PhysicsLink(const DeviceIdentifier &id, const nlohmann::json &data)
    : DeviceInterface(id),
      PropertyTemplate(json_property_serializer_t::readProperties(data, DefPosition, DefRotation,
                                                                  DefLinearVelocity, DefAngularVelocity))
{}

const PhysicsLink::vec3_t &PhysicsLink::position() const
{
	return this->getPropertyByName<Position, vec3_t>();
}

void PhysicsLink::setPosition(const PhysicsLink::vec3_t &position)
{
	this->getPropertyByName<Position, vec3_t>() = position;
}

const PhysicsLink::quat_t &PhysicsLink::rotation() const
{
	return this->getPropertyByName<Rotation, quat_t>();
}

void PhysicsLink::setRotation(const PhysicsLink::quat_t &rotation)
{
	this->getPropertyByName<Rotation, quat_t>() = rotation;
}

const PhysicsLink::vec3_t &PhysicsLink::linVel() const
{
	return this->getPropertyByName<LinearVelocity, vec3_t>();
}

void PhysicsLink::setLinVel(const PhysicsLink::vec3_t &linVel)
{
	this->getPropertyByName<LinearVelocity, vec3_t>() = linVel;
}

const PhysicsLink::vec3_t &PhysicsLink::angVel() const
{
	return this->getPropertyByName<AngularVelocity, vec3_t>();
}

void PhysicsLink::setAngVel(const PhysicsLink::vec3_t &angVel)
{
	this->getPropertyByName<AngularVelocity, vec3_t>() = angVel;
}
