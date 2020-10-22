#include <string>

#include <grpcpp/health_check_service_interface.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"

EngineGrpcServer::EngineGrpcServer()
{
    std::string serverAddress("0.0.0.0:9002");

    grpc::EnableDefaultHealthCheckService(true);
}