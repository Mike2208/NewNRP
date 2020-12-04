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
