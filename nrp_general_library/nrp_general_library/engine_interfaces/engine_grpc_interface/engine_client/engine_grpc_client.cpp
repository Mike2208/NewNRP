#include <string>

#include <grpcpp/grpcpp.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_client/engine_grpc_client.h"

EngineGrpcClient::EngineGrpcClient()
{
    std::string serverAddress("0.0.0.0:9002");

    _channel = grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials());
}