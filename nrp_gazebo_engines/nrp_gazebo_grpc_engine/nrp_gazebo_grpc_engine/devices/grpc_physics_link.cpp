//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_gazebo_grpc_engine/devices/grpc_physics_link.h"


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsLink>(const PhysicsLink &dev)
{
	GRPCDevice dat = serializeID<GRPCDevice>(dev.id());
	dat.dev().mutable_link()->InitAsDefaultInstance();

	dat.dev().mutable_link()->add_position(dev.position()[0]);
	dat.dev().mutable_link()->add_position(dev.position()[1]);
	dat.dev().mutable_link()->add_position(dev.position()[2]);

	dat.dev().mutable_link()->add_rotation(dev.rotation()[0]);
	dat.dev().mutable_link()->add_rotation(dev.rotation()[1]);
	dat.dev().mutable_link()->add_rotation(dev.rotation()[2]);
	dat.dev().mutable_link()->add_rotation(dev.rotation()[3]);

	dat.dev().mutable_link()->add_linearvelocity(dev.linVel()[0]);
	dat.dev().mutable_link()->add_linearvelocity(dev.linVel()[1]);
	dat.dev().mutable_link()->add_linearvelocity(dev.linVel()[2]);

	dat.dev().mutable_link()->add_angularvelocity(dev.angVel()[0]);
	dat.dev().mutable_link()->add_angularvelocity(dev.angVel()[1]);
	dat.dev().mutable_link()->add_angularvelocity(dev.angVel()[2]);

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
