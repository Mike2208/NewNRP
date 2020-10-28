#include <iostream>

#include <gtest/gtest.h>

#include <dummy.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"
#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"
#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_client/engine_grpc_client.h"

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

        dummy::DummyRequest _data;
        dummy::DummyReply   _reply;
};

TEST(EngineGrpc, BASIC)
{
    EngineGrpcServer server;
    EngineGrpcClient client;

    ASSERT_EQ(client.getChannelStatus(), grpc_connectivity_state::GRPC_CHANNEL_IDLE);

    server.startServer();

    ASSERT_EQ(client.connect(), grpc_connectivity_state::GRPC_CHANNEL_READY);

    server.shutdownServer();
}

TEST(EngineGrpc, RegisterDevices)
{
    EngineGrpcServer server;

    TestGrpcDeviceController dev1(DeviceIdentifier("dev1", "test", "test"));

    ASSERT_EQ(server.getNumRegisteredDevices(), 0);
    server.registerDevice("dev1", &dev1);
    ASSERT_EQ(server.getNumRegisteredDevices(), 1);
}

TEST(EngineGrpc, SetDeviceData)
{
    EngineGrpcServer server;

    const std::string deviceName = "device";

    TestGrpcDeviceController device(DeviceIdentifier("dev1", "test", "test"));

    dummy::DummyRequest request;
    request.set_name("test");

    server.registerDevice(deviceName, &device);

    server.setDeviceData(deviceName, request);

    ASSERT_EQ(device._data.name(), "test");
}

TEST(EngineGrpc, GetDeviceData)
{
    EngineGrpcServer server;

    const std::string deviceName = "device";

    TestGrpcDeviceController device(DeviceIdentifier("dev1", "test", "test"));

    dummy::DummyRequest request;
    request.set_name("test");

    server.registerDevice(deviceName, &device);

    server.setDeviceData(deviceName, request);
    const dummy::DummyReply * reply = dynamic_cast<const dummy::DummyReply *>(server.getDeviceData(deviceName));
    ASSERT_EQ(reply->numcalls(), 1); 
}