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

#include "nrp_gazebo_grpc_engine/devices/grpc_physics_camera.h"


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsCamera>(const PhysicsCamera &dev)
{
	GRPCDevice msg = serializeID<GRPCDevice>(dev.id());
	msg.dev().mutable_camera()->InitAsDefaultInstance();
	msg.dev().mutable_camera()->set_imagedata(dev.imageData().data(), dev.imageData().size());
	msg.dev().mutable_camera()->set_imagedepth(dev.imagePixelSize());
	msg.dev().mutable_camera()->set_imageheight(dev.imageHeight());
	msg.dev().mutable_camera()->set_imagewidth(dev.imageWidth());

	return msg;
}

template<>
PhysicsCamera DeviceSerializerMethods<GRPCDevice>::deserialize<PhysicsCamera>(DeviceIdentifier &&devID, deserialization_t data)
{
	return PhysicsCamera(std::move(devID), PhysicsCamera::property_template_t(data->camera().imageheight(),
	                                                                          data->camera().imagewidth(),
	                                                                          data->camera().imagedepth(),
	                                                                          PhysicsCamera::cam_data_t(data->camera().imagedata().begin(),
	                                                                                                    data->camera().imagedata().end())));
}
