/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

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
