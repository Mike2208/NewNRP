#ifndef ENGINE_GRPC_SERVER_H
#define ENGINE_GRPC_SERVER_H

#include <string>
#include <map>

#include <grpcpp/grpcpp.h>
#include <nlohmann/json.hpp>

#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.grpc.pb.h"

#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"

using EngineGrpc::EngineGrpcService;

/*!
 * \brief Abstract class for Engine server with gRPC support
 *
 * The class provides a base for implementing an Engine server with gRPC
 * as middleware. All RPC services are implemented. Derived classes are responsible
 * for implementing simulation initialization, shutdown and run step methods.
 */
class EngineGrpcServer : public EngineGrpcService::Service
{
    public:

        using mutex_t = std::timed_mutex;
        using lock_t  = std::unique_lock<EngineGrpcServer::mutex_t>;

        EngineGrpcServer();
        EngineGrpcServer(const std::string address);

        /*!
         * \brief Constructor
         *
         * \param[in] serverAddress Address of the gRPC server
         * \param[in] engineName    Name of the simulation engine
         * \param[in] registrationAddress Should be removed
         */
        // TODO registrationAddress isn't needed
        EngineGrpcServer(const std::string &serverAddress, const std::string &engineName, const std::string &registrationAddress);

        /*!
         * \brief Destructor
         */
        virtual ~EngineGrpcServer();

        /*!
         * \brief Starts the gRPC server in synchronous mode
         */
        void startServer();

        /*!
         * \brief Starts the gRPC server in asynchronous mode
         */
        void startServerAsync();

        /*!
         * \brief Shutdowns the gRPC server
         */
        void shutdownServer();

        /*!
         * \brief Indicates whether the gRPC server is currently running
         */
        bool isServerRunning() const;

        /*!
         * \brief Returns address of the gRPC server
         */
        const std::string serverAddress() const;

        /*!
         * \brief Registers a device controller with the given name in the engine
         *
         * \param[in] deviceName       Name of the device to be registered
         * \param[in] deviceController Pointer to the device controller object that's supposed to be
         *                             registered in the engine
         */
        void registerDevice(const std::string & deviceName, EngineGrpcDeviceControllerInterface *interface);

        // TODO used only in tests, try to remove it?
        unsigned getNumRegisteredDevices();

    protected:
        mutex_t                       _deviceLock;
        void clearRegisteredDevices();

    private:

        /*!
         * \brief Address of the gRPC server
         */
        std::string _serverAddress;

        /*!
         * \brief Name of the simulation engine
         *
         * Must be the same on the server and the client side. It should be imprinted
         * in the device metadata, which allows for additional consistency checks.
         */
        std::string _engineName;

        /*!
         * \brief Pointer to the gRPC server object
         */
        std::unique_ptr<grpc::Server> _server;

        /*!
         * \brief Indicates whether the gRPC server is currently running
         */
        bool _isServerRunning;

        /*!
         * \brief Map of device names and device controllers used by the engine
         */
         std::map<std::string, EngineGrpcDeviceControllerInterface*> _devicesControllers;

        /*!
         * \brief Initializes the simulation
         *
         * \param[in] data       Simulation configuration data
         * \param[in] deviceLock ???
         */
        virtual void initialize(const nlohmann::json &data, EngineGrpcServer::lock_t &deviceLock) = 0;

        /*!
         * \brief Shutdowns the simulation
         *
         * \param[in] data Additional data
         */
        virtual void shutdown(const nlohmann::json &data) = 0;

        /*!
         * \brief Runs a single simulation loop step
         *
         * \param[in] timeStep Time step by which the simulation should be advanced
         *
         * \return Engine time after running the step
         */
        virtual float runLoopStep(const float timeStep) = 0;

        /*!
         * \brief Initializes the simulation
         *
         * The function implements the init method of the EngineGrpcService.
         * It acts as a wrapper around the virtual initialize method, which should initialize the simulation.
         * On error, it will return a status object with error message and grpc::StatusCode::CANCELLED error code.
         *
         * \param      context Pointer to gRPC server context structure
         * \param[in]  request Pointer to protobuf init request message. Contains simulation config in JSON format.
         * \param[out] reply   Pointer to protobuf init reply message. Currently no data is returned.
         *
         * \return gRPC request status
         */
        grpc::Status init(grpc::ServerContext * context, const EngineGrpc::InitRequest * request, EngineGrpc::InitReply * reply) override;

        /*!
         * \brief Shutdowns the simulation
         *
         * The function implements the shutdown method of the EngineGrpcService.
         * It acts as a wrapper around the virtual shutdown method, which should shutdown the simulation.
         * On error, it will return a status object with error message and grpc::StatusCode::CANCELLED error code.
         *
         * \param      context Pointer to gRPC server context structure
         * \param[in]  request Pointer to protobuf shutdown request message. Contains shutdown parameters in JSON format.
         * \param[out] reply   Pointer to protobuf shutdown reply message. Currently no data is returned.
         *
         * \return gRPC request status
         */
        grpc::Status shutdown(grpc::ServerContext * context, const EngineGrpc::ShutdownRequest * request, EngineGrpc::ShutdownReply * reply) override;

        /*!
         * \brief Runs a single loop step of the simulation
         *
         * The function implements the runLoopStep method of the EngineGrpcService.
         * It acts as a wrapper around the virtual runLoopStep method, which should run a single step of the simulation.
         * On error, it will return a status object with error message and grpc::StatusCode::CANCELLED error code.
         *
         * \param      context Pointer to gRPC server context structure
         * \param[in]  request Pointer to protobuf runLoopStep request message. Contains simulation time step.
         * \param[out] reply   Pointer to protobuf runLoopStep reply message. Contains engine time.
         *
         * \return gRPC request status
         */
        grpc::Status runLoopStep(grpc::ServerContext * context, const EngineGrpc::RunLoopStepRequest * request, EngineGrpc::RunLoopStepReply * reply) override;

        /*!
         * \brief Sets received data into proper devices
         *
         * The function implements the setDevice method of the EngineGrpcService.
         * It acts as a wrapper around the virtual setDeviceData method, which should set received data into proper devices.
         * On error, it will return a status object with error message and grpc::StatusCode::CANCELLED error code.
         *
         * \param      context Pointer to gRPC server context structure
         * \param[in]  request Pointer to protobuf setDevice request message. Contains device data and metadata
         * \param[out] reply   Pointer to protobuf setDevice reply message. Currently no data is returned.
         *
         * \return gRPC request status
         */
        grpc::Status setDevice(grpc::ServerContext * context, const EngineGrpc::SetDeviceRequest * request, EngineGrpc::SetDeviceReply * reply) override;

        /*!
         * \brief Gets data from requested devices
         *
         * The function implements the getDevice method of the EngineGrpcService.
         * It acts as a wrapper around the virtual getDeviceData method, which should get data from requested devices.
         * On error, it will return a status object with error message and grpc::StatusCode::CANCELLED error code.
         *
         * \param      context Pointer to gRPC server context structure
         * \param[in]  request Pointer to protobuf setDevice request message. Contains metadata of requested devices.
         * \param[out] reply   Pointer to protobuf setDevice reply message. Contains device data and metadata.
         *
         * \return gRPC request status
         */
        grpc::Status getDevice(grpc::ServerContext * context, const EngineGrpc::GetDeviceRequest * request, EngineGrpc::GetDeviceReply * reply) override;

        virtual void setDeviceData(const EngineGrpc::SetDeviceRequest & data);
        virtual void getDeviceData(const EngineGrpc::GetDeviceRequest & request, EngineGrpc::GetDeviceReply * reply);

        /*!
         * \brief Helper function for handling errors inside Remote Procedure Calls (RPCs)
         *
         * \param[in] contextMessage Name of the RPC that called the helper
         * \param[in] errorMessage   Error message that will be passed to the gRPC client
         *
         * \return gRPC request status, containing the error message and grpc::StatusCode::CANCELLED error code
         */
        grpc::Status handleGrpcError(const std::string & contextMessage, const std::string & errorMessage);
};

#endif // ENGINE_GRPC_SERVER_H
