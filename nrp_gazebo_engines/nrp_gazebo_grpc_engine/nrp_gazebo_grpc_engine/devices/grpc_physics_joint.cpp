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
