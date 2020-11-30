#include "nrp_gazebo_devices/physics_link.h"


const PhysicsLink::vec3_t &PhysicsLink::position() const
{
	return this->getPropertyByName<Position>();
}

void PhysicsLink::setPosition(const PhysicsLink::vec3_t &position)
{
	this->getPropertyByName<Position>() = position;
}

const PhysicsLink::quat_t &PhysicsLink::rotation() const
{
	return this->getPropertyByName<Rotation>();
}

void PhysicsLink::setRotation(const PhysicsLink::quat_t &rotation)
{
	this->getPropertyByName<Rotation>() = rotation;
}

const PhysicsLink::vec3_t &PhysicsLink::linVel() const
{
	return this->getPropertyByName<LinearVelocity>();
}

void PhysicsLink::setLinVel(const PhysicsLink::vec3_t &linVel)
{
	this->getPropertyByName<LinearVelocity>() = linVel;
}

const PhysicsLink::vec3_t &PhysicsLink::angVel() const
{
	return this->getPropertyByName<AngularVelocity>();
}

void PhysicsLink::setAngVel(const PhysicsLink::vec3_t &angVel)
{
	this->getPropertyByName<AngularVelocity>() = angVel;
}
