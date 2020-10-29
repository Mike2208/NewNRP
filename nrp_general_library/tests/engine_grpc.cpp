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
            return &_reply;
        }

        virtual void setData(const google::protobuf::Message & data) override
        {
            _data.CopyFrom(data);

            this->_reply.set_numcalls(_reply.numcalls() + 1);
        }

        EngineGrpc::DummyRequest _data;
        EngineGrpc::DummyReply   _reply;
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

class TestEngineGrpcClient
        : public EngineGrpcClient<TestEngineGrpcClient, TestEngineJSONConfig>
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
        {}

        virtual ~TestEngineGrpcServer() override = default;

        nlohmann::json initialize(const nlohmann::json &data) override
        {
            return nlohmann::json({{"status", "success"}, {"original", data}});
        }

        nlohmann::json shutdown(const nlohmann::json &data) override
        {
            return nlohmann::json({{"status", "shutdown"}, {"original", data}});
        }
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
    timeStep = 1.0f;
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

TEST(EngineGrpc, SetDeviceData)
{
    TestEngineGrpcServer server;

    const std::string deviceName = "device";

    TestGrpcDeviceController device(DeviceIdentifier("dev1", "test", "test"));

    EngineGrpc::DummyRequest request;
    request.set_name("test");

    server.registerDevice(deviceName, &device);

    server.setDeviceData(deviceName, request);

    ASSERT_EQ(device._data.name(), "test");
}

TEST(EngineGrpc, GetDeviceData)
{
    TestEngineGrpcServer server;

    const std::string deviceName = "device";

    TestGrpcDeviceController device(DeviceIdentifier("dev1", "test", "test"));

    EngineGrpc::DummyRequest request;
    request.set_name("test");

    server.registerDevice(deviceName, &device);

    server.setDeviceData(deviceName, request);
    const EngineGrpc::DummyReply * reply = dynamic_cast<const EngineGrpc::DummyReply *>(server.getDeviceData(deviceName));
    ASSERT_EQ(reply->numcalls(), 1); 
}