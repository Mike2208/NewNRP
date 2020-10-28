#ifndef ENGINE_GRPC_CLIENT_H
#define ENGINE_GRPC_CLIENT_H

#include <grpcpp/grpcpp.h>

#include <dummy.grpc.pb.h>

class EngineGrpcClient
{
    public:

        EngineGrpcClient();

        grpc_connectivity_state getChannelStatus();
        grpc_connectivity_state connect();

        void sendInitCommand();

    private:

        std::shared_ptr<grpc::Channel>      _channel;
        std::unique_ptr<dummy::Dummy::Stub> _stub;
};

#endif // ENGINE_GRPC_CLIENT_H