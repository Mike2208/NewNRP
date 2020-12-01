#include "nrp_gazebo_grpc_engine/devices/grpc_physics_link.h"


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsLink>(const PhysicsLink &dev)
{
	GRPCDevice dat = serializeID(dev.id());
	dat.dev().mutable_link()->InitAsDefaultInstance();

	dat.dev().mutable_link()->set_position(0, dev.position()[0]);
	dat.dev().mutable_link()->set_position(1, dev.position()[1]);
	dat.dev().mutable_link()->set_position(2, dev.position()[2]);

	dat.dev().mutable_link()->set_rotation(0, dev.rotation()[0]);
	dat.dev().mutable_link()->set_rotation(1, dev.rotation()[1]);
	dat.dev().mutable_link()->set_rotation(2, dev.rotation()[2]);
	dat.dev().mutable_link()->set_rotation(3, dev.rotation()[3]);

	dat.dev().mutable_link()->set_linearvelocity(0, dev.linVel()[0]);
	dat.dev().mutable_link()->set_linearvelocity(1, dev.linVel()[1]);
	dat.dev().mutable_link()->set_linearvelocity(2, dev.linVel()[2]);

	dat.dev().mutable_link()->set_angularvelocity(0, dev.angVel()[0]);
	dat.dev().mutable_link()->set_angularvelocity(1, dev.angVel()[1]);
	dat.dev().mutable_link()->set_angularvelocity(2, dev.angVel()[2]);

	return dat;
}

template<>
PhysicsLink DeviceSerializerMethods<GRPCDevice>::deserialize<PhysicsLink>(DeviceIdentifier &&devID, deserialization_t data)
{
	return PhysicsLink(std::move(devID), PhysicsLink::property_template_t(
	                   PhysicsLink::vec3_t({data->link().position(0), data->link().position(1), data->link().position(2)}),
	                   PhysicsLink::quat_t({data->link().rotation(0), data->link().rotation(1), data->link().rotation(2), data->link().rotation(3)}),
	                   PhysicsLink::vec3_t({data->link().linearvelocity(0), data->link().linearvelocity(1), data->link().linearvelocity(2)}),
	                   PhysicsLink::vec3_t({data->link().angularvelocity(0), data->link().angularvelocity(1), data->link().angularvelocity(2)})));
}
