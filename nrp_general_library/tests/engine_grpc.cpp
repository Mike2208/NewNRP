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

        virtual void getData(EngineGrpc::GetDeviceMessage *) override
        {
            //reply->mutable_deviceid()->set_devicename(_setMessage.deviceid().devicename());
            //reply->mutable_deviceid()->set_devicetype(_setMessage.deviceid().devicetype());
        }

        virtual void setData(const google::protobuf::Message & data) override
        {
            _setMessage.CopyFrom(data);
        }

        EngineGrpc::SetDeviceMessage _setMessage;
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

class TestGrpcDeviceInterface1
    : public DeviceInterface
{
    public:

        static constexpr std::string_view TypeName = "test_type1";

        TestGrpcDeviceInterface1(const DeviceIdentifier &devID)
            : DeviceInterface(devID)
        {}

        TestGrpcDeviceInterface1(const DeviceIdentifier &devID, const EngineGrpc::GetDeviceMessage &)
            : TestGrpcDeviceInterface1(devID)
        {
            // TODO
        }
};

class TestGrpcDeviceInterface2
    : public DeviceInterface
{
    public:

        static constexpr std::string_view TypeName = "test_type2";

        TestGrpcDeviceInterface2(const DeviceIdentifier &devID)
            : DeviceInterface(devID)
        {}

        TestGrpcDeviceInterface2(const DeviceIdentifier &devID, const EngineGrpc::GetDeviceMessage &)
            : TestGrpcDeviceInterface2(devID)
        {
            // TODO
        }
};

class TestEngineGrpcClient
        : public EngineGrpcClient<TestEngineGrpcClient, TestEngineJSONConfig, TestGrpcDeviceInterface1, TestGrpcDeviceInterface2>
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

        nlohmann::json initialize(const nlohmann::json &data, EngineGrpcServer::lock_t &) override
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
    const std::string deviceType = "test_type1";
    const std::string engineName = "c";

    client.engineName() = engineName;

    DeviceIdentifier         devId(deviceName, deviceType, engineName);
    TestGrpcDeviceInterface1 dev1(devId);             // Client side
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

    const std::string deviceName = "TestDevice";
    const std::string deviceType = "TestType";
    const std::string engineName = "TestEngine";

    TestGrpcDeviceController device(DeviceIdentifier("dev1", deviceType, engineName));

    EngineGrpc::SetDeviceRequest setRequest;
    EngineGrpc::GetDeviceRequest getRequest;
    EngineGrpc::GetDeviceReply   getReply;
    auto req = setRequest.add_request();
    req->mutable_deviceid()->set_devicename(deviceName);

    auto devId = getRequest.add_deviceid();
    devId->set_devicename(deviceName);

    server.registerDevice(deviceName, &device);

    server.setDeviceData(setRequest);
    server.getDeviceData(getRequest, &getReply);

    ASSERT_EQ(getReply.reply(0).deviceid().devicename(), deviceName);
    ASSERT_EQ(getReply.reply(0).deviceid().devicetype(), deviceType);
    ASSERT_EQ(getReply.reply(0).deviceid().enginename(), engineName);
}

TEST(EngineGrpc, GetDeviceData2)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Client sends a request to the server

    std::vector<DeviceInterface*> input_devices;

    const std::string deviceName = "a";
    const std::string deviceType = "test_type2";
    const std::string engineName = "c";

    client.engineName() = engineName;

    DeviceIdentifier         devId(deviceName, deviceType, engineName);
    TestGrpcDeviceInterface2 dev1(devId);             // Client side
    TestGrpcDeviceController deviceController(devId); // Server side

    server.registerDevice(deviceName, &deviceController);

    input_devices.push_back(&dev1);

    server.startServer();
    client.handleInputDevices(input_devices);

    EngineInterface::device_identifiers_t deviceIdentifiers;
    deviceIdentifiers.insert(devId);

    const auto output = client.getOutputDevices(deviceIdentifiers);

    ASSERT_EQ(output.size(), 1);
    ASSERT_EQ(output.at(0)->name(),       deviceName);
    ASSERT_EQ(output.at(0)->type(),       deviceType);
    ASSERT_EQ(output.at(0)->engineName(), engineName);
}

TEST(EngineGrpc, GetDeviceData3)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Client sends a request to the server

    std::vector<DeviceInterface*> input_devices;

    const std::string engineName = "c";

    const std::string deviceName1 = "a";
    const std::string deviceType1 = "test_type1";
    const std::string deviceName2 = "b";
    const std::string deviceType2 = "test_type2";

    client.engineName() = engineName;

    DeviceIdentifier         devId1(deviceName1, deviceType1, engineName);
    DeviceIdentifier         devId2(deviceName2, deviceType2, engineName);
    TestGrpcDeviceInterface1 dev1(devId1);              // Client side
    TestGrpcDeviceInterface2 dev2(devId2);              // Client side
    TestGrpcDeviceController deviceController1(devId1); // Server side
    TestGrpcDeviceController deviceController2(devId2); // Server side

    server.registerDevice(deviceName1, &deviceController1);
    server.registerDevice(deviceName2, &deviceController2);

    input_devices.push_back(&dev1);
    input_devices.push_back(&dev2);

    server.startServer();
    client.handleInputDevices(input_devices);

    EngineInterface::device_identifiers_t deviceIdentifiers;
    deviceIdentifiers.insert(devId1);
    deviceIdentifiers.insert(devId2);

    const auto output = client.getOutputDevices(deviceIdentifiers);

    ASSERT_EQ(output.size(), 2);
    ASSERT_EQ(output.at(0)->engineName(), engineName);
    ASSERT_EQ(output.at(1)->engineName(), engineName);

    if(output.at(0)->type().compare(deviceType1) == 0)
    {
        ASSERT_EQ(output.at(0)->name(), deviceName1);
        ASSERT_EQ(output.at(1)->name(), deviceName2);
        ASSERT_EQ(output.at(1)->type(), deviceType2);
    }
    else
    {
        ASSERT_EQ(output.at(0)->name(), deviceName2);
        ASSERT_EQ(output.at(1)->name(), deviceName1);
        ASSERT_EQ(output.at(1)->type(), deviceType1);
    }
}

// EOF
