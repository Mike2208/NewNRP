#ifndef GRPC_PHYSICS_JOINT_H
#define GRPC_PHYSICS_JOINT_H

#include "nrp_gazebo_devices/physics_joint.h"
#include "nrp_grpc_engine_protocol/device_interfaces/grpc_device_serializer.h"


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<PhysicsJoint>(const PhysicsJoint &dev);

template<>
PhysicsJoint DeviceSerializerMethods<GRPCDevice>::deserialize<PhysicsJoint>(DeviceIdentifier &&devID, deserialization_t data);


#endif // GRPC_PHYSICS_JOINT_H
