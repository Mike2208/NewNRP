#ifndef ENGINE_JSON_DEVICE_CONTROLLER_H
#define ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_general_library/engine_interfaces/engine_device_controller.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

template<DEVICE_C DEVICE>
using EngineJSONDeviceController = EngineDeviceController<nlohmann::json, DEVICE>;

#endif // ENGINE_JSON_DEVICE_CONTROLLER_H
