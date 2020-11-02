#ifndef ENGINE_GRPC_CLIENT_H
#define ENGINE_GRPC_CLIENT_H

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/time.h>
#include <nlohmann/json.hpp>

#include <engine_grpc.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"

template<class ENGINE, ENGINE_CONFIG_C ENGINE_CONFIG>
class EngineGrpcClient
    : public Engine<ENGINE, ENGINE_CONFIG>
{
    public:

        EngineGrpcClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
            : Engine<ENGINE, ENGINE_CONFIG>(config, std::move(launcher))
        {
            std::string serverAddress("0.0.0.0:9002");

            _channel = grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials());
            _stub    = EngineGrpc::EngineGrpcServiceInterface::NewStub(_channel);

            _prevEngineTime = 0.0f;
        }

        grpc_connectivity_state getChannelStatus()
        {
            return _channel->GetState(false);
        }

        grpc_connectivity_state connect()
        {
            _channel->GetState(true);
            _channel->WaitForConnected(gpr_time_add(
            gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_seconds(10, GPR_TIMESPAN)));
            return _channel->GetState(false);
        }

        void sendInitCommand(const nlohmann::json & data)
        {
            EngineGrpc::InitRequest  request;
            EngineGrpc::InitReply    reply;
            grpc::ClientContext      context;

            request.set_json(data.dump());

            grpc::Status status = _stub->init(&context, request, &reply);

            if(!status.ok())
            {
                const auto errMsg = "Engine server initialization failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
                throw std::runtime_error(errMsg);
            }
        }

        void sendShutdownCommand(const nlohmann::json & data)
        {
            EngineGrpc::ShutdownRequest request;
            EngineGrpc::ShutdownReply   reply;
            grpc::ClientContext         context;

            request.set_json(data.dump());

            grpc::Status status = _stub->shutdown(&context, request, &reply);

            if(!status.ok())
            {
                const auto errMsg = "Engine server shutdown failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
                throw std::runtime_error(errMsg);
            }
        }

        float sendRunLoopStepCommand(const float timeStep)
        {
            EngineGrpc::RunLoopStepRequest request;
            EngineGrpc::RunLoopStepReply   reply;
            grpc::ClientContext       context;

            request.set_timestep(timeStep);

            grpc::Status status = _stub->runLoopStep(&context, request, &reply);

            if(!status.ok())
            {
               const auto errMsg = "Engine server runLoopStep failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
               throw std::runtime_error(errMsg);
            }

            const float engineTime = reply.enginetime();

            if(engineTime < 0.0f)
            {
               const auto errMsg = "Invalid engine time (should be greater than 0): " + std::to_string(engineTime);
               throw std::runtime_error(errMsg);
            }

            if(engineTime < this->_prevEngineTime)
            {
                const auto errMsg = "Invalid engine time (should be greater than previous time): " + std::to_string(engineTime) + ", previous: " + std::to_string(this->_prevEngineTime);
                throw std::runtime_error(errMsg);
            }

            this->_prevEngineTime = engineTime;

            return engineTime;
        }

        float getEngineTime() const override
        {
            return 0.0f;
        }

        virtual typename EngineInterface::step_result_t runLoopStep(float timeStep) override
        {
            return EngineInterface::SUCCESS;
        }

        virtual typename EngineInterface::RESULT waitForStepCompletion(float timeOut) override
        {
            return EngineInterface::SUCCESS;
        }

        virtual typename EngineInterface::RESULT handleInputDevices(const typename EngineInterface::device_inputs_t &inputDevices) override
        {
            EngineGrpc::SetDeviceRequest request;
            EngineGrpc::SetDeviceReply   reply;
            grpc::ClientContext          context;

            for(const auto &device : inputDevices)
            {
                auto r = request.add_request();
                r->set_devicename(device->name());
            }

            grpc::Status status = _stub->setDevice(&context, request, &reply);

            if(!status.ok())
            {
                const auto errMsg = "Engine server handleInputDevices failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
                throw std::runtime_error(errMsg);
            }

            return EngineInterface::SUCCESS;
        }

        virtual typename EngineInterface::device_outputs_t getOutputDevices(const typename EngineInterface::device_identifiers_t &deviceIdentifiers) override
        {
            EngineGrpc::GetDeviceRequest request;
            EngineGrpc::GetDeviceReply   reply;
            grpc::ClientContext          context;

            for(const auto &devID : deviceIdentifiers)
            {
                request.add_devicename(devID.Name);
            }

            grpc::Status status = _stub->getDevice(&context, request, &reply);

            if(!status.ok())
            {
                const auto errMsg = "Engine server getOutputDevices failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
                throw std::runtime_error(errMsg);
            }

            return this->getDeviceInterfacesFromProtobuf(reply);
        }

        typename EngineInterface::device_outputs_t getDeviceInterfacesFromProtobuf(const EngineGrpc::GetDeviceReply & reply)
        {
            typename EngineInterface::device_outputs_t interfaces;
            interfaces.reserve(reply.reply_size());

            for(int i = 0; i < reply.reply_size(); i++)
            {
                const auto r = reply.reply(i);

                DeviceInterfaceSharedPtr newDevice(new DeviceInterface(r.devicename(), "", ""));

                interfaces.push_back(newDevice);
            }

            return interfaces;
        }

    private:

        std::shared_ptr<grpc::Channel>                                _channel;
        std::unique_ptr<EngineGrpc::EngineGrpcServiceInterface::Stub> _stub;

        float _prevEngineTime;
};

#endif // ENGINE_GRPC_CLIENT_H

// EOF
