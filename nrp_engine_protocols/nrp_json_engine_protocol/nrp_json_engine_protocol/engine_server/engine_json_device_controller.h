#ifndef ENGINE_JSON_DEVICE_CONTROLLER_H
#define ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_general_library/engine_interfaces/engine_device_controller.h"
#include "nrp_json_engine_protocol/device_interfaces/json_device_serializer.h"

using EngineJSONDeviceControllerInterface = EngineDeviceControllerInterface<nlohmann::json>;

template<DEVICE_C DEVICE>
using EngineJSONDeviceController = EngineDeviceController<nlohmann::json, DEVICE>;

#endif // ENGINE_JSON_DEVICE_CONTROLLER_H
