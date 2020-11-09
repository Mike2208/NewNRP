#include "nrp_gazebo_grpc_engine/devices/physics_link.h"


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

PhysicsLink::PhysicsLink(const DeviceIdentifier &id, const EngineGrpc::GetDeviceMessage &data)
	: PhysicsLink(id)
{
	assert(data.link().position_size() == 3);

	PhysicsLink::vec3_t pos { data.link().position(0),
	                          data.link().position(1),
							  data.link().position(2) };

	this->setPosition(pos);

	assert(data.link().rotation_size() == 4);

	PhysicsLink::quat_t rot { data.link().rotation(0),
							  data.link().rotation(1),
							  data.link().rotation(2),
							  data.link().rotation(3) };

	this->setRotation(rot);

	assert(data.link().linearvelocity_size() == 3);

	PhysicsLink::vec3_t lin { data.link().linearvelocity(0),
	                          data.link().linearvelocity(1),
							  data.link().linearvelocity(2) };

	this->setLinVel(lin);

	assert(data.link().angularvelocity_size() == 3);

	PhysicsLink::vec3_t ang { data.link().angularvelocity(0),
	                          data.link().angularvelocity(1),
							  data.link().angularvelocity(2) };

	this->setAngVel(ang);
}

void PhysicsLink::serialize(EngineGrpc::SetDeviceMessage *) const
{
	// Do nothing
}

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
