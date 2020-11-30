#include "nrp_gazebo_grpc_engine/devices/physics_joint.h"

PhysicsJointConst::FloatNan::FloatNan(float val)
    : _val(val)
{}

PhysicsJoint::PhysicsJoint(const std::string &name)
    : PhysicsJoint(DeviceIdentifier(name, "", TypeName.data()))
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id)
    : DeviceInterface(id),
      PropertyTemplate(NAN, NAN, NAN)
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id, const nlohmann::json &data)
    : DeviceInterface(id),
      PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, NAN, NAN, NAN))
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id, const EngineGrpc::GetDeviceMessage &data)
	: PhysicsJoint(id)
{
	this->setPosition(data.joint().position());
	this->setVelocity(data.joint().velocity());
	this->setEffort(data.joint().effort());
}

void PhysicsJoint::serialize(EngineGrpc::SetDeviceMessage * request) const
{z
	request->mutable_joint()->set_position(this->position());
	request->mutable_joint()->set_velocity(this->velocity());
	request->mutable_joint()->set_effort(this->effort());
}

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
		throw std::out_of_range(std::string("Couldn't find JSON attribute ") + name.data() + " during deserialization");
}

template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsJoint>(const PhysicsJoint &dev)
{
	GRPCDevice data = serializeID<GRPCDevice>(dev.id());
	data.dev().mutable_joint()->InitAsDefaultInstance();
	data.dev().mutable_joint()->set_position(dev.position());
	data.dev().mutable_joint()->set_velocity(dev.velocity());
	data.dev().mutable_joint()->set_effort(dev.effort());

	return data;
}

template<>
PhysicsJoint DeviceSerializerMethods<GRPCDevice>::deserialize<PhysicsJoint>(DeviceIdentifier &&devID, deserialization_t data)
{
	return PhysicsJoint(std::move(devID), data->joint().position(), data->joint().velocity(), data->joint().effort());
}
