#include <string>
#include <type_traits>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_server.h"

grpc::Status EngineGrpcServer::handleGrpcError(const std::string & contextMessage, const std::string & errorMessage)
{
    std::cerr << contextMessage << std::endl;
    std::cerr << errorMessage   << std::endl;

    // Pass the error message to the client

    grpc::Status status(grpc::StatusCode::CANCELLED, errorMessage);

    return status;
}

grpc::Status EngineGrpcServer::init(grpc::ServerContext * , const EngineGrpc::InitRequest * request, EngineGrpc::InitReply *)
{
    try
    {
        EngineGrpcServer::lock_t lock(this->_deviceLock);

        nlohmann::json requestJson = nlohmann::json::parse(request->json());

        // Run engine-specific initialization function

        this->initialize(requestJson, lock);
    }
    catch(const std::exception &e)
    {
        return handleGrpcError("Error while executing initialization", e.what());
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::shutdown(grpc::ServerContext * , const EngineGrpc::ShutdownRequest * request, EngineGrpc::ShutdownReply *)
{
    try
    {
        EngineGrpcServer::lock_t lock(this->_deviceLock);

        nlohmann::json requestJson = nlohmann::json::parse(request->json());

        // Run engine-specifi shutdown function

        this->shutdown(requestJson);
    }
    catch(const std::exception &e)
    {
        return handleGrpcError("Error while executing shutdown", e.what());
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::runLoopStep(grpc::ServerContext * , const EngineGrpc::RunLoopStepRequest * request, EngineGrpc::RunLoopStepReply * reply)
{
    try
    {
        EngineGrpcServer::lock_t lock(this->_deviceLock);

        int64_t engineTime = (this->runLoopStep(SimulationTime(request->timestep()))).count();

        reply->set_enginetime(engineTime);
    }
    catch(const std::exception &e)
    {
        return handleGrpcError("Error while executing runLoopStep", e.what());
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::setDevice(grpc::ServerContext * , const EngineGrpc::SetDeviceRequest * request, EngineGrpc::SetDeviceReply *)
{
    try
    {
        this->setDeviceData(*request);
    }
    catch(const std::exception &e)
    {
        return handleGrpcError("Error while executing setDevice", e.what());
    }

    return grpc::Status::OK;
}

grpc::Status EngineGrpcServer::getDevice(grpc::ServerContext * , const EngineGrpc::GetDeviceRequest * request, EngineGrpc::GetDeviceReply * reply)
{
    try
    {
        this->getDeviceData(*request, reply);
    }
    catch(const std::exception &e)
    {
        return handleGrpcError("Error while executing getDevice", e.what());
    }

    return grpc::Status::OK;
}

EngineGrpcServer::EngineGrpcServer()
{
    this->_serverAddress   = "0.0.0.0:9002";
    this->_isServerRunning = false;

    grpc::EnableDefaultHealthCheckService(true);
}

EngineGrpcServer::EngineGrpcServer(const std::string &serverAddress, const std::string &engineName, const std::string &)
    : EngineGrpcServer(serverAddress)
{
    this->_engineName    = engineName;
    this->_serverAddress = serverAddress;
}

EngineGrpcServer::EngineGrpcServer(const std::string address)
{
    this->_serverAddress   = address;
    this->_isServerRunning = false;

    grpc::EnableDefaultHealthCheckService(true);
}

EngineGrpcServer::~EngineGrpcServer()
{
	this->shutdownServer();
}

bool EngineGrpcServer::isServerRunning() const
{
    return this->_isServerRunning;
}

const std::string EngineGrpcServer::serverAddress() const
{
    return this->_serverAddress;
}

void EngineGrpcServer::startServerAsync()
{
    this->startServer();
}

void EngineGrpcServer::startServer()
{
	if(!this->_isServerRunning)
	{
        grpc::ServerBuilder builder;
        builder.AddListeningPort(_serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(this);

        this->_server = builder.BuildAndStart();
        // TODO Should we use a memory barrier here?
		this->_isServerRunning = true;
	}
}

void EngineGrpcServer::shutdownServer()
{
	if(this->_isServerRunning)
	{
		this->_server->Shutdown();
        // TODO Should we use a memory barrier here?
		this->_isServerRunning = false;
	}
}

void EngineGrpcServer::registerDevice(const std::string &deviceName, EngineGrpcDeviceControllerInterface *interface)
{
    EngineGrpcServer::lock_t lock(this->_deviceLock);
    this->_devicesControllers.emplace(deviceName, interface);
}

unsigned EngineGrpcServer::getNumRegisteredDevices()
{
    return this->_devicesControllers.size();
}

void EngineGrpcServer::clearRegisteredDevices()
{
    // TODO Check if it's true
	// Do not lock scope. This method is called from the route handlers, which should already have locked down access.
	//EngineJSONServer::lock_t lock(this->_deviceLock);

	this->_devicesControllers.clear();
}

void EngineGrpcServer::setDeviceData(const EngineGrpc::SetDeviceRequest & data)
{
    EngineGrpcServer::lock_t lock(this->_deviceLock);

    const auto numDevices = data.request_size();

    for(int i = 0; i < numDevices; i++)
    {
        const auto r = data.request(i);
		const auto &devInterface = this->_devicesControllers.find(r.deviceid().devicename());

        if(devInterface != _devicesControllers.end())
        {
			devInterface->second->handleDeviceData(&r);
        }
        else
        {
            const auto errorMessage = "Device " + r.deviceid().devicename() + " is not registered in engine " + this->_engineName;
            throw std::invalid_argument(errorMessage);
        }
    }
}

void EngineGrpcServer::getDeviceData(const EngineGrpc::GetDeviceRequest & request, EngineGrpc::GetDeviceReply * reply)
{
    EngineGrpcServer::lock_t lock(this->_deviceLock);

    const auto numDevices = request.deviceid_size();

    for(int i = 0; i < numDevices; i++)
    {
		const auto &devInterface = this->_devicesControllers.find(request.deviceid(i).devicename());

        if(devInterface != _devicesControllers.end())
        {
			auto r = reply->add_reply();
			*r = devInterface->second->getDeviceInformation();
        }
        else
        {
            const auto errorMessage = "Device " + request.deviceid(i).devicename() + " is not registered in engine " + this->_engineName;
            throw std::invalid_argument(errorMessage);
        }
    }
}

// EOF
