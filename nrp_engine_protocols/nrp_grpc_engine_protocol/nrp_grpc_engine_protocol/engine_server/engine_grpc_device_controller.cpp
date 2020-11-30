#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"

EngineGrpcDeviceController::EngineGrpcDeviceController(const DeviceIdentifier &id)
    : DeviceIdentifier(id)
{}

EngineGrpcDeviceController::~EngineGrpcDeviceController() = default;