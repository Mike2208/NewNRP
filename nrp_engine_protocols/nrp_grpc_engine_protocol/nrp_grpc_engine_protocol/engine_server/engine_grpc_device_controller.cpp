#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"

EngineGrpcDeviceController::EngineGrpcDeviceController(const DeviceIdentifier &id)
    : DeviceIdentifier(id)
{}

EngineGrpcDeviceController::~EngineGrpcDeviceController() = default;