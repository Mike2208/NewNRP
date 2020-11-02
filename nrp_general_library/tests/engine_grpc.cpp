#include <iostream>

#include <gtest/gtest.h>

#include <engine_grpc.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"
#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"
#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_client/engine_grpc_client.h"
#include "nrp_general_library/process_launchers/process_launcher_basic.h"

class TestGrpcDeviceController : public EngineGrpcDeviceController
{
    public:

        TestGrpcDeviceController(const DeviceIdentifier &devID) : EngineGrpcDeviceController(devID) {}

        virtual const google::protobuf::Message * getData() override
        {
            _getMessage.Clear();

            _getMessage.mutable_deviceid()->set_devicename(_setMessage.deviceid().devicename());

            return &_getMessage;
        }

        virtual void setData(const google::protobuf::Message & data) override
        {
            _setMessage.CopyFrom(data);
        }

        EngineGrpc::SetDeviceMessage _setMessage;
        EngineGrpc::GetDeviceMessage _getMessage;
};

class TestEngineJSONConfig
        : public EngineJSONConfig<TestEngineJSONConfig, PropNames<> >
{
    public:
        static constexpr FixedString ConfigType = "TestEngineConfig";

        TestEngineJSONConfig(EngineConfigConst::config_storage_t &config)
            : EngineJSONConfig(config)
        {}
};

class TestGrpcDeviceInterface
    : public DeviceInterface
{
    public:

        TestGrpcDeviceInterface(const DeviceIdentifier &devID)
            : DeviceInterface(devID)
        {}
};

class TestEngineGrpcClient
        : public EngineGrpcClient<TestEngineGrpcClient, TestEngineJSONConfig, TestGrpcDeviceInterface>
{
    public:
        TestEngineGrpcClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
            : EngineGrpcClient(config, std::move(launcher))
        {}

        virtual ~TestEngineGrpcClient() override = default;

        RESULT initialize() override
        {
            this->sendInitCommand("test");

            return RESULT::SUCCESS;
        }

        RESULT shutdown() override
        {
            this->sendShutdownCommand("test");

            return RESULT::SUCCESS;
        }
};

class TestEngineGrpcServer
    : public EngineGrpcServer
{
    public:
        template<class ...T>
        TestEngineGrpcServer(T &&...properties)
            : EngineGrpcServer(std::forward<T>(properties)...)
        {
            this->_time = 0.0f;
        }

        virtual ~TestEngineGrpcServer() override = default;

        nlohmann::json initialize(const nlohmann::json &data) override
        {
            return nlohmann::json({{"status", "success"}, {"original", data}});
        }

        nlohmann::json shutdown(const nlohmann::json &data) override
        {
            return nlohmann::json({{"status", "shutdown"}, {"original", data}});
        }

        float runLoopStep(const float timeStep)
        {
            _time += timeStep;

            return _time;
        }

    private:

        float _time;
};

TEST(EngineGrpc, BASIC)
{
    // TODO This one has a linking issue, fix it!

    /*EngineGrpcServer server;

    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    ASSERT_EQ(client.getChannelStatus(), grpc_connectivity_state::GRPC_CHANNEL_IDLE);

    server.startServer();

    ASSERT_EQ(client.connect(), grpc_connectivity_state::GRPC_CHANNEL_READY);

    server.shutdownServer();*/
}

TEST(EngineGrpc, InitCommand)
{
    TestEngineGrpcServer server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    ASSERT_THROW(client.sendInitCommand("initCommand"), std::runtime_error);

    server.startServer();
    ASSERT_NO_THROW(client.sendInitCommand("initCommand"));
}

TEST(EngineGrpc, ShutdownCommand)
{
    TestEngineGrpcServer server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    ASSERT_THROW(client.sendShutdownCommand("shutdownCommand"), std::runtime_error);

    server.startServer();
    ASSERT_NO_THROW(client.sendShutdownCommand("shutdownCommand"));
}

TEST(EngineGrpc, RunLoopStepCommand)
{
    TestEngineGrpcServer server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    server.startServer();

    float timeStep = 0.1f;
    ASSERT_NEAR(client.sendRunLoopStepCommand(timeStep), timeStep, 0.0001);

    timeStep = -0.1f;
    ASSERT_THROW(client.sendRunLoopStepCommand(timeStep), std::runtime_error);

    timeStep = 2.0f;
    ASSERT_NO_THROW(client.sendRunLoopStepCommand(timeStep));
    timeStep = -1.0f;
    ASSERT_THROW(client.sendRunLoopStepCommand(timeStep), std::runtime_error);
}

TEST(EngineGrpc, RegisterDevices)
{
    TestEngineGrpcServer server;

    TestGrpcDeviceController dev1(DeviceIdentifier("dev1", "test", "test"));

    ASSERT_EQ(server.getNumRegisteredDevices(), 0);
    server.registerDevice("dev1", &dev1);
    ASSERT_EQ(server.getNumRegisteredDevices(), 1);
}

TEST(EngineGrpc, SetDeviceData1)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Only server method

    const std::string deviceName = "device";

    TestGrpcDeviceController device(DeviceIdentifier("test", "test", "test"));

    EngineGrpc::SetDeviceRequest request;
    auto r = request.add_request();
    r->mutable_deviceid()->set_devicename(deviceName);

    server.registerDevice(deviceName, &device);

    server.setDeviceData(request);

    ASSERT_EQ(device._setMessage.deviceid().devicename(), deviceName);
}

TEST(EngineGrpc, SetDeviceData2)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Client sends a request to the server

    std::vector<DeviceInterface*> input_devices;

    const std::string deviceName = "a";
    const std::string deviceType = "b";
    const std::string engineName = "c";

    DeviceIdentifier         devId(deviceName, deviceType, engineName);
    TestGrpcDeviceInterface  dev1(devId);             // Client side
    TestGrpcDeviceController deviceController(devId); // Server side

    server.registerDevice(deviceName, &deviceController);

    input_devices.push_back(&dev1);

    server.startServer();
    client.handleInputDevices(input_devices);

    ASSERT_EQ(deviceController._setMessage.deviceid().devicename(), deviceName);
}

TEST(EngineGrpc, GetDeviceData1)
{
    TestEngineGrpcServer server;

    const std::string deviceName = "device";

    TestGrpcDeviceController device(DeviceIdentifier("dev1", "test", "test"));

    EngineGrpc::SetDeviceRequest setRequest;
    EngineGrpc::GetDeviceRequest getRequest;
    auto req = setRequest.add_request();
    req->mutable_deviceid()->set_devicename(deviceName);

    auto devId = getRequest.add_deviceid();
    devId->set_devicename(deviceName);

    server.registerDevice(deviceName, &device);

    server.setDeviceData(setRequest);
    const auto response = server.getDeviceData(getRequest);

    ASSERT_EQ(response->reply(0).deviceid().devicename(), deviceName);
}

TEST(EngineGrpc, GetDeviceData2)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Client sends a request to the server

    std::vector<DeviceInterface*> input_devices;

    const std::string deviceName = "a";
    const std::string deviceType = "b";
    const std::string engineName = "c";

    DeviceIdentifier         devId(deviceName, deviceType, engineName);
    TestGrpcDeviceInterface  dev1(devId);             // Client side
    TestGrpcDeviceController deviceController(devId); // Server side

    server.registerDevice(deviceName, &deviceController);

    input_devices.push_back(&dev1);

    server.startServer();
    client.handleInputDevices(input_devices);

    EngineInterface::device_identifiers_t deviceIdentifiers;
    deviceIdentifiers.insert(devId);

    const auto output = client.getOutputDevices(deviceIdentifiers);

    ASSERT_EQ(output.at(0)->name(), deviceName);
}

// EOF
