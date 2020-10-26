#include <iostream>

#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"
#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_client/engine_grpc_client.h"

TEST(EngineGrpc, BASIC)
{
    EngineGrpcServer server;
    EngineGrpcClient client;

    ASSERT_EQ(client.connect(), grpc_connectivity_state::GRPC_CHANNEL_READY);
}