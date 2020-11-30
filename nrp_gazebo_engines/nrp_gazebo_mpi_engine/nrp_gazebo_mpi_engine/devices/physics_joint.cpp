#include "nrp_gazebo_mpi_engine/devices/physics_joint.h"

#include <type_traits>

PhysicsJointConst::FloatNan::FloatNan(float val)
    : _val(val)
{}

PhysicsJoint::PhysicsJoint(const std::string &name)
    : PhysicsJoint(DeviceIdentifier(name, "", TypeName.data()))
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id)
    : DeviceInterface(id),
      PropertyTemplate(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN())
{}

float PhysicsJoint::position() const
{
	return this->getPropertyByName<Position>();
}

void PhysicsJoint::setPosition(float position)
{
	this->getPropertyByName<Position>() = position;
}

float PhysicsJoint::velocity() const
{
	return this->getPropertyByName<Velocity>();
}

void PhysicsJoint::setVelocity(float velocity)
{
	this->getPropertyByName<Velocity>() = velocity;
}

float PhysicsJoint::effort() const
{
	return this->getPropertyByName<Effort>();
}

void PhysicsJoint::setEffort(float effort)
{
	this->getPropertyByName<Effort>() = effort;
}
