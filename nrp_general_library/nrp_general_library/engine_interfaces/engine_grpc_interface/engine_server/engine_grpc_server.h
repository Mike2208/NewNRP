#ifndef ENGINE_GRPC_SERVER_H
#define ENGINE_GRPC_SERVER_H

#include <grpcpp/grpcpp.h>

class EngineGrpcServer
{
    public:

        EngineGrpcServer();

    private:

    std::unique_ptr<grpc::Server> _server;
};

#endif // ENGINE_GRPC_SERVER_H