#ifndef ENGINE_GRPC_SERVER_H
#define ENGINE_GRPC_SERVER_H

#include <string>
#include <map>

#include <grpcpp/grpcpp.h>

#include <engine_grpc.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"

using EngineGrpc::EngineGrpcServiceInterface;
using EngineGrpc::DummyRequest;
using EngineGrpc::DummyReply;

class EngineGrpcServer : public EngineGrpcServiceInterface::Service
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

        grpc::Status dummy(grpc::ServerContext * context, const DummyRequest * request, DummyReply * reply) override;
        grpc::Status init(grpc::ServerContext * context, const EngineGrpc::InitRequest * request, EngineGrpc::InitReply * reply) override;
        grpc::Status shutdown(grpc::ServerContext * context, const EngineGrpc::ShutdownRequest * request, EngineGrpc::ShutdownReply * reply) override;
        grpc::Status runLoopStep(grpc::ServerContext * context, const EngineGrpc::RunLoopStepRequest * request, EngineGrpc::RunLoopStepReply * reply) override;

    private:

        std::string                   _serverAddress;
        std::unique_ptr<grpc::Server> _server;
        bool                          _isServerRunning;

        std::map<std::string, EngineGrpcDeviceController*> _devicesControllers;
};

#endif // ENGINE_GRPC_SERVER_H