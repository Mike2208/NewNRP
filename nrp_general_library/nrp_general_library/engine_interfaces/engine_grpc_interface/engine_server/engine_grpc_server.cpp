#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"

grpc::Status EngineGrpcService::dummy(grpc::ServerContext * context, const DummyRequest * request, DummyReply * reply)
{
    reply->set_numcalls(0);
    return grpc::Status::OK;
}

grpc::Status EngineGrpcService::init(grpc::ServerContext * context, const EngineGrpc::InitRequest * request, EngineGrpc::InitReply * reply)
{
    return grpc::Status::OK;
}

grpc::Status EngineGrpcService::shutdown(grpc::ServerContext * context, const EngineGrpc::ShutdownRequest * request, EngineGrpc::ShutdownReply * reply)
{
    return grpc::Status::OK;
}

grpc::Status EngineGrpcService::runLoopStep(grpc::ServerContext * context, const EngineGrpc::RunLoopStepRequest * request, EngineGrpc::RunLoopStepReply * reply)
{
    reply->set_enginetime(request->timestep());

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

void EngineGrpcServer::registerDevice(const std::string &deviceName, EngineGrpcDeviceController *interface)
{
    this->_devicesControllers.emplace(deviceName, interface);
}

unsigned EngineGrpcServer::getNumRegisteredDevices()
{
    return this->_devicesControllers.size();
}

void EngineGrpcServer::setDeviceData(const std::string & deviceName, const google::protobuf::Message & data)
{
    const auto devInterface = this->_devicesControllers.find(deviceName);

    if(devInterface == _devicesControllers.end())
    {
        // TODO Error handling for dev not found
    }

    devInterface->second->setData(data);
}

const google::protobuf::Message * EngineGrpcServer::getDeviceData(const std::string & deviceName)
{
    const auto devInterface = this->_devicesControllers.find(deviceName);

    if(devInterface == _devicesControllers.end())
    {
        // TODO Error handling for dev not found
    }

    return devInterface->second->getData();
}