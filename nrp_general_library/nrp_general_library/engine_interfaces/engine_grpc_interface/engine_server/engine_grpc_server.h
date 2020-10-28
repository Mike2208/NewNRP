#ifndef ENGINE_GRPC_SERVER_H
#define ENGINE_GRPC_SERVER_H

#include <string>
#include <map>

#include <grpcpp/grpcpp.h>

#include <dummy.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"

using dummy::Dummy;
using dummy::DummyRequest;
using dummy::DummyReply;

// Logic and data behind the server's behavior.
class DummyServiceImpl final : public Dummy::Service
{
    grpc::Status dummy(grpc::ServerContext * context, const DummyRequest * request, DummyReply * reply) override;
};

class EngineGrpcServer
{
    public:

        EngineGrpcServer();
        virtual ~EngineGrpcServer();

        void startServer();
        void shutdownServer();

        void registerDevice(const std::string & deviceName, EngineGrpcDeviceController * interface);
        unsigned getNumRegisteredDevices();

        void setDeviceData(const std::string & deviceName, const google::protobuf::Message & data);
        const google::protobuf::Message * getDeviceData(const std::string & deviceName);

    private:

        std::string                   _serverAddress;
        DummyServiceImpl              _service;
        std::unique_ptr<grpc::Server> _server;
        bool                          _isServerRunning;

        std::map<std::string, EngineGrpcDeviceController*> _devicesControllers;
};

#endif // ENGINE_GRPC_SERVER_H