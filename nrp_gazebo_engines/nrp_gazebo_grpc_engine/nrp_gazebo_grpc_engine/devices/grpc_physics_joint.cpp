#include "nrp_gazebo_grpc_engine/devices/grpc_physics_joint.h"


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
	return PhysicsJoint(std::move(devID), PhysicsJoint::property_template_t(data->joint().position(), data->joint().velocity(), data->joint().effort()));
}
