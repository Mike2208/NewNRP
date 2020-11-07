#include "nrp_gazebo_json_engine/devices/physics_joint.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

PhysicsJointConst::FloatNan::FloatNan(float val)
    : _val(val)
{}

PhysicsJoint::PhysicsJoint(const std::string &name)
    : PhysicsJoint(DeviceIdentifier(name, TypeName.data(), ""))
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id)
    : DeviceInterface(id),
      PropertyTemplate(NAN, NAN, NAN)
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id, const nlohmann::json &data)
    : DeviceInterface(id),
      PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, NAN, NAN, NAN))
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

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PhysicsJointConst::FloatNan &property)
{
	return nlohmann::json(static_cast<float>(property));
}

template<>
PhysicsJointConst::FloatNan JSONPropertySerializerMethods::deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name)
{
	const auto dataIterator(data.find(name.data()));
	if(dataIterator != data.end())
	{
		if(dataIterator->is_number())
			return dataIterator->get<float>();
		else
			return NAN;
	}
	else
		throw NRPException::logCreate(std::string("Couldn't find JSON attribute ") + name.data() + " during deserialization");
}
