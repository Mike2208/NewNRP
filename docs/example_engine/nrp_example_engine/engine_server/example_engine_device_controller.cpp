#include "nrp_example_engine/engine_server/example_engine_device_controller.h"

ExampleEngineDeviceControllerInterface::ExampleEngineDeviceControllerInterface(const DeviceIdentifier &devID)
    : _id(devID)
{}

ExampleEngineDeviceController<ExampleDevice>::~ExampleEngineDeviceController() = default;
