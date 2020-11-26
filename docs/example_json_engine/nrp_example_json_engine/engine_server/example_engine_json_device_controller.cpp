#include "nrp_example_json_engine/engine_server/example_engine_json_device_controller.h"

ExampleEngineJSONDeviceControllerInterface::ExampleEngineJSONDeviceControllerInterface(const DeviceIdentifier &devID)
    : _id(devID)
{}

ExampleEngineJSONDeviceController<ExampleDevice>::~ExampleEngineJSONDeviceController() = default;
