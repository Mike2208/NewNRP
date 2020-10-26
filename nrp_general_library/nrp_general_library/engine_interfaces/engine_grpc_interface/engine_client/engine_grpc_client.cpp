#include <string>
#include <iostream>

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/time.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_client/engine_grpc_client.h"

EngineGrpcClient::EngineGrpcClient()
{
    std::string serverAddress("0.0.0.0:9002");

    _channel = grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials());
}

grpc_connectivity_state EngineGrpcClient::getChannelStatus()
{
    return _channel->GetState(false);
}

grpc_connectivity_state EngineGrpcClient::connect()
{
    _channel->GetState(true);
    _channel->WaitForConnected(gpr_time_add(
    gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(10, GPR_TIMESPAN)));
    return _channel->GetState(false);
}