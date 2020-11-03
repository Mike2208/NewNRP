#ifndef ENGINE_GRPC_SERVER_H
#define ENGINE_GRPC_SERVER_H

#include <string>
#include <map>

#include <grpcpp/grpcpp.h>
#include <nlohmann/json.hpp>

#include <engine_grpc.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"

using EngineGrpc::EngineGrpcServiceInterface;

class EngineGrpcServer : public EngineGrpcServiceInterface::Service
{
    public:

        using mutex_t = std::timed_mutex;
        using lock_t = std::unique_lock<EngineGrpcServer::mutex_t>;

        EngineGrpcServer();
        EngineGrpcServer(const std::string address);
        virtual ~EngineGrpcServer();

        void startServer();
        void startServerAsync();
        void shutdownServer();

        const std::string serverAddress();

        void registerDevice(const std::string & deviceName, EngineGrpcDeviceController * interface);
        unsigned getNumRegisteredDevices();

        void setDeviceData(const EngineGrpc::SetDeviceRequest & data);
        const EngineGrpc::GetDeviceReply * getDeviceData(const EngineGrpc::GetDeviceRequest & data);

        virtual nlohmann::json initialize(const nlohmann::json &data, EngineGrpcServer::lock_t &deviceLock) = 0;
        virtual nlohmann::json shutdown(const nlohmann::json &data) = 0;
        virtual float runLoopStep(const float timeStep) = 0;

        grpc::Status init(grpc::ServerContext * context, const EngineGrpc::InitRequest * request, EngineGrpc::InitReply * reply) override;
        grpc::Status shutdown(grpc::ServerContext * context, const EngineGrpc::ShutdownRequest * request, EngineGrpc::ShutdownReply * reply) override;
        grpc::Status runLoopStep(grpc::ServerContext * context, const EngineGrpc::RunLoopStepRequest * request, EngineGrpc::RunLoopStepReply * reply) override;
        grpc::Status setDevice(grpc::ServerContext * context, const EngineGrpc::SetDeviceRequest * request, EngineGrpc::SetDeviceReply * reply) override;
        grpc::Status getDevice(grpc::ServerContext * context, const EngineGrpc::GetDeviceRequest * request, EngineGrpc::GetDeviceReply * reply) override;

    protected:
        mutex_t                       _deviceLock;

    private:

        std::string                   _serverAddress;
        std::unique_ptr<grpc::Server> _server;
        bool                          _isServerRunning;

        std::map<std::string, EngineGrpcDeviceController*> _devicesControllers;

        EngineGrpc::GetDeviceReply _getReply;
};

#endif // ENGINE_GRPC_SERVER_H