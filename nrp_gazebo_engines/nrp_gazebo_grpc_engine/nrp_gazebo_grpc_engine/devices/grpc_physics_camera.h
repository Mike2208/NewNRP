#ifndef GRPC_PHYSICS_CAMERA_H
#define GRPC_PHYSICS_CAMERA_H

#include "nrp_gazebo_devices/physics_camera.h"
#include "nrp_grpc_engine_protocol/device_interfaces/grpc_device_serializer.h"


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsCamera>(const PhysicsCamera &dev);

template<>
PhysicsCamera DeviceSerializerMethods<GRPCDevice>::deserialize<PhysicsCamera>(DeviceIdentifier &&devID, deserialization_t data);

#endif // GRPC_PHYSICS_CAMERA_H
