#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.h"

grpc::Status EngineGrpcServer::init(grpc::ServerContext * context, const EngineGrpc::InitRequest * request, EngineGrpc::InitReply * reply)
{
    try
    {
        // Run initialization function
        reply->set_json(this->initialize(request->json()).dump());
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error while executing initialization\n";
        std::cerr << e.what();

        return grpc::Status::CANCELLED;
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::shutdown(grpc::ServerContext * context, const EngineGrpc::ShutdownRequest * request, EngineGrpc::ShutdownReply * reply)
{
    try
    {
        // Run shutdown function
        reply->set_json(this->shutdown(request->json()).dump());
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error while executing shutdown\n";
        std::cerr << e.what();

        return grpc::Status::CANCELLED;
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::runLoopStep(grpc::ServerContext * context, const EngineGrpc::RunLoopStepRequest * request, EngineGrpc::RunLoopStepReply * reply)
{
    try
    {
        reply->set_enginetime(this->runLoopStep(request->timestep()));
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error while executing runLoopStep\n";
        std::cerr << e.what();

        return grpc::Status::CANCELLED;
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::setDevice(grpc::ServerContext * context, const EngineGrpc::SetDeviceRequest * request, EngineGrpc::SetDeviceReply * reply)
{
    try
    {
        this->setDeviceData(*request);
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error while executing runLoopStep\n";
        std::cerr << e.what();

        return grpc::Status::CANCELLED;
    }

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
        builder.RegisterService(this);

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

void EngineGrpcServer::setDeviceData(const EngineGrpc::SetDeviceRequest & data)
{
    const auto numDevices = data.devicename_size();

    for(unsigned i = 0; i < numDevices; i++)
    {
        const auto devInterface = this->_devicesControllers.find(data.devicename(i));

        if(devInterface != _devicesControllers.end())
        {
            devInterface->second->setData(data);
            // TODO Error handling for dev not found
        }
    }
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