#ifndef ENGINE_GRPC_CLIENT_H
#define ENGINE_GRPC_CLIENT_H

#include <grpcpp/grpcpp.h>

class EngineGrpcClient
{
    public:

        EngineGrpcClient();

        grpc_connectivity_state getChannelStatus();
        grpc_connectivity_state connect();

    private:

        std::shared_ptr<grpc::Channel> _channel;
};

#endif // ENGINE_GRPC_CLIENT_H