#ifndef GRPC_PHYSICS_LINK_H
#define GRPC_PHYSICS_LINK_H

#include "nrp_gazebo_devices/physics_link.h"
#include "nrp_grpc_engine_protocol/device_interfaces/grpc_device_serializer.h"


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsLink>(const PhysicsLink &dev);

template<>
PhysicsLink DeviceSerializerMethods<GRPCDevice>::deserialize<PhysicsLink>(DeviceIdentifier &&devID, deserialization_t data);


#endif // GRPC_PHYSICS_LINK_H
