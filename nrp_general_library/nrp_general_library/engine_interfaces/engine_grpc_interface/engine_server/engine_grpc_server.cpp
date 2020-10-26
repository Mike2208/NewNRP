#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"

grpc::Status DummyServiceImpl::dummy(grpc::ServerContext * context, const DummyRequest * request, DummyReply * reply)
{
    reply->set_message(request->name());
    return grpc::Status::OK;
}

EngineGrpcServer::EngineGrpcServer()
{
    std::string serverAddress("0.0.0.0:9002");

    grpc::EnableDefaultHealthCheckService(true);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&_service);

    _server = builder.BuildAndStart();
}