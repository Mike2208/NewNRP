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
    this->_serverAddress   = "0.0.0.0:9002";
    this->_isServerRunning = false;

    grpc::EnableDefaultHealthCheckService(true);
}

EngineGrpcServer::~EngineGrpcServer()
{
	this->shutdownServer();
}

void EngineGrpcServer::startServer()
{
	if(!this->_isServerRunning)
	{
        grpc::ServerBuilder builder;
        builder.AddListeningPort(_serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(&_service);

        this->_server = builder.BuildAndStart();
		this->_isServerRunning = true;
	}
}

void EngineGrpcServer::shutdownServer()
{
	if(this->_isServerRunning)
	{
		this->_server->Shutdown();
		this->_isServerRunning = false;
	}
}