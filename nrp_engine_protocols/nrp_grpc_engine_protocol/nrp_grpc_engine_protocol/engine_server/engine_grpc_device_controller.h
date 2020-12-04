#ifndef ENGINE_GRPC_DEVICE_CONTROLLER_H
#define ENGINE_GRPC_DEVICE_CONTROLLER_H

#include "nrp_general_library/engine_interfaces/engine_device_controller.h"
#include "nrp_grpc_engine_protocol/device_interfaces/grpc_device_serializer.h"
#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.grpc.pb.h"


using EngineGrpcDeviceControllerInterface = EngineDeviceControllerInterface<GRPCDevice>;

template<DEVICE_C DEVICE>
using EngineGrpcDeviceController = EngineDeviceController<GRPCDevice, DEVICE>;

template<template<class> class DEVICE_CONTROLLER>
using GrpcDeviceControlSerializer = DEVICE_CONTROLLER<GRPCDevice>;

#endif // ENGINE_GRPC_DEVICE_CONTROLLER_H
