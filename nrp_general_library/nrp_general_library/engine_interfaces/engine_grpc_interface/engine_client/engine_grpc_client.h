#ifndef ENGINE_GRPC_CLIENT_H
#define ENGINE_GRPC_CLIENT_H

#include <future>

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/time.h>
#include <nlohmann/json.hpp>

#include "engine_grpc.grpc.pb.h"

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_grpc_interface/device_interfaces/grpc_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"

template<class ENGINE, ENGINE_CONFIG_C ENGINE_CONFIG, DEVICE_C ...DEVICES>
class EngineGrpcClient
    : public Engine<ENGINE, ENGINE_CONFIG>
{
    public:

        EngineGrpcClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
            : Engine<ENGINE, ENGINE_CONFIG>(config, std::move(launcher))
        {
            std::string serverAddress = this->engineConfig()->engineServerAddress();

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
            return this->_engineTime;
        }

        virtual typename EngineInterface::step_result_t runLoopStep(float timeStep) override
        {
            this->_loopStepThread = std::async(std::launch::async, std::bind(&EngineGrpcClient::sendRunLoopStepCommand, this, timeStep));
            return EngineInterface::SUCCESS;
        }

        virtual typename EngineInterface::RESULT waitForStepCompletion(float timeOut) override
        {
            // If thread state is invalid, loop thread has completed and waitForStepCompletion was called once before
            if(!this->_loopStepThread.valid())
            {
                return EngineInterface::SUCCESS;
            }

            // Wait until timeOut has passed
            if(timeOut > 0)
            {
                if(this->_loopStepThread.wait_for(std::chrono::duration<double>(timeOut)) != std::future_status::ready)
                    return EngineInterface::ERROR;
            }
            else
                this->_loopStepThread.wait();

            this->_engineTime = this->_loopStepThread.get();
            return EngineInterface::SUCCESS;
        }

        virtual typename EngineInterface::RESULT handleInputDevices(const typename EngineInterface::device_inputs_t &inputDevices) override
        {
            EngineGrpc::SetDeviceRequest request;
            EngineGrpc::SetDeviceReply   reply;
            grpc::ClientContext          context;

            for(const auto &device : inputDevices)
            {
                if(device->engineName().compare(this->engineName()) == 0)
                {
                    auto r = request.add_request();
                    this->getProtoFromSingleDeviceInterface<DEVICES...>(*device, r);
                }
            }

            grpc::Status status = _stub->setDevice(&context, request, &reply);

            if(!status.ok())
            {
                const auto errMsg = "Engine server handleInputDevices failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
                throw std::runtime_error(errMsg);
            }

            return EngineInterface::SUCCESS;
        }

        template<class DEVICE, class ...REMAINING_DEVICES>
        inline void getProtoFromSingleDeviceInterface(const DeviceInterface &device, EngineGrpc::SetDeviceMessage * request) const
        {
            if(DEVICE::TypeName.compare(device.type()) == 0)
            {
                request->mutable_deviceid()->set_devicename(device.name());
                request->mutable_deviceid()->set_devicetype(device.type());
                request->mutable_deviceid()->set_enginename(device.engineName());

                device.serialize(request);

                // Early return

                return;
            }

            // If device classess are left to check, go through them. If all device classes have been checked without proper result, throw an error

            if constexpr (sizeof...(REMAINING_DEVICES) > 0)
            {
                this->getProtoFromSingleDeviceInterface<REMAINING_DEVICES...>(device, request);
            }
            else
            {
                throw std::logic_error("Could not process given device of type " + device.type());
            }
        }

        virtual typename EngineInterface::device_outputs_t getOutputDevices(const typename EngineInterface::device_identifiers_t &deviceIdentifiers) override
        {
            EngineGrpc::GetDeviceRequest request;
            EngineGrpc::GetDeviceReply   reply;
            grpc::ClientContext          context;

            for(const auto &devID : deviceIdentifiers)
            {
                if(this->engineName().compare(devID.EngineName) == 0)
                {
                    auto r = request.add_deviceid();

                    r->set_devicename(devID.Name);
                    r->set_devicetype(devID.Type);
                    r->set_enginename(devID.EngineName);
                }
            }

            grpc::Status status = _stub->getDevice(&context, request, &reply);

            if(!status.ok())
            {
                const auto errMsg = "Engine server getOutputDevices failed: " + status.error_message() + " (" + std::to_string(status.error_code()) + ")";
                throw std::runtime_error(errMsg);
            }

            return this->getDeviceInterfacesFromProto(reply);
        }

        typename EngineInterface::device_outputs_t getDeviceInterfacesFromProto(const EngineGrpc::GetDeviceReply & reply)
        {
            typename EngineInterface::device_outputs_t interfaces;
            interfaces.reserve(reply.reply_size());

            for(int i = 0; i < reply.reply_size(); i++)
            {
                interfaces.push_back(this->getSingleDeviceInterfaceFromProto<DEVICES...>(reply.reply(i)));
            }

            return interfaces;
        }

        template<class DEVICE, class ...REMAINING_DEVICES>
        inline DeviceInterfaceConstSharedPtr getSingleDeviceInterfaceFromProto(const EngineGrpc::GetDeviceMessage &deviceData) const
        {
            if(DEVICE::TypeName.compare(deviceData.deviceid().devicetype()) == 0)
            {
                DeviceIdentifier devId(deviceData.deviceid().devicename(),
                                       deviceData.deviceid().devicetype(),
                                       deviceData.deviceid().enginename());

                DeviceInterfaceSharedPtr newDevice(new DEVICE(devId, deviceData));
                // TODO Why is this done here?
                newDevice->setEngineName(this->engineName());

                return newDevice;
            }

            // If device classess are left to check, go through them. If all device classes have been checked without proper result, throw an error
            if constexpr (sizeof...(REMAINING_DEVICES) > 0)
            {
                return this->getSingleDeviceInterfaceFromProto<REMAINING_DEVICES...>(deviceData);
            }
            else
            {
                throw std::logic_error("Could not process given device of type " + deviceData.deviceid().devicetype());
            }
        }

        //using dcm_t = DeviceConversionMechanism<EngineGrpc::SetDeviceMessage, const EngineGrpc::GetDeviceMessage, DEVICES...>;
        //dcm_t _dcm;

    private:

        std::shared_ptr<grpc::Channel>                                _channel;
        std::unique_ptr<EngineGrpc::EngineGrpcServiceInterface::Stub> _stub;

        float _prevEngineTime = 0.0f;
        float _engineTime     = 0.0f;
        std::future<float> _loopStepThread;
};

#endif // ENGINE_GRPC_CLIENT_H

// EOF
