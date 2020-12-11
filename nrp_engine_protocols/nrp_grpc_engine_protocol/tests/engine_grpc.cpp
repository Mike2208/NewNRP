//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include <iostream>

#include <gtest/gtest.h>

#include "nrp_general_library/process_launchers/process_launcher_basic.h"
#include "nrp_grpc_engine_protocol/config/engine_grpc_config.h"
#include "nrp_grpc_engine_protocol/device_interfaces/grpc_device_serializer.h"
#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"
#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_server.h"
#include "nrp_grpc_engine_protocol/engine_client/engine_grpc_client.h"
#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.grpc.pb.h"


void testSleep(unsigned sleepMs)
{
    std::chrono::milliseconds timespan(sleepMs);
    std::this_thread::sleep_for(timespan);
}

class TestGrpcDeviceInterface1
    : public Device<TestGrpcDeviceInterface1, "test_type1", PropNames<> >
{
	public:
		TestGrpcDeviceInterface1(DeviceIdentifier &&devID, property_template_t &&props = property_template_t())
		    : Device(std::move(devID), std::move(props))
		{}
};

class TestGrpcDeviceController
        : public EngineGrpcDeviceController<TestGrpcDeviceInterface1>
{
    public:

		TestGrpcDeviceController(DeviceIdentifier &&devID)
		    : EngineGrpcDeviceController<TestGrpcDeviceInterface1>(std::move(devID)),
		      _dev(DeviceIdentifier(*this))
		{}

		virtual void handleDeviceDataCallback(TestGrpcDeviceInterface1 &&data) override
		{	this->_dev = std::move(data);	}

		virtual const TestGrpcDeviceInterface1 *getDeviceInformationCallback() override
		{	return &this->_dev;	}

	private:
		TestGrpcDeviceInterface1 _dev;
};

class TestEngineGRPCConfig
        : public EngineGRPCConfig<TestEngineGRPCConfig, PropNames<> >
{
    public:
        static constexpr FixedString ConfigType = "TestEngineConfig";

		TestEngineGRPCConfig(EngineConfigConst::config_storage_t &config)
		    : EngineGRPCConfig(config)
        {}
};


template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<TestGrpcDeviceInterface1>(const TestGrpcDeviceInterface1 &dev)
{
	GRPCDevice data = serializeID<GRPCDevice>(dev.id());

	return data;
}

template<>
TestGrpcDeviceInterface1 DeviceSerializerMethods<GRPCDevice>::deserialize<TestGrpcDeviceInterface1>(DeviceIdentifier &&devID, deserialization_t)
{
	return TestGrpcDeviceInterface1(std::move(devID));
}


class TestGrpcDeviceInterface2
    : public Device<TestGrpcDeviceInterface2, "test_type2", PropNames<> >
{
    public:
		TestGrpcDeviceInterface2(DeviceIdentifier &&devID, property_template_t &&props = property_template_t())
		    : Device(std::move(devID), std::move(props))
		{}

		TestGrpcDeviceInterface2(const DeviceIdentifier &devID, const EngineGrpc::DeviceMessage &)
		    : TestGrpcDeviceInterface2(DeviceIdentifier(devID))
		{}
};

template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serialize<TestGrpcDeviceInterface2>(const TestGrpcDeviceInterface2 &dev)
{
	GRPCDevice data = serializeID<GRPCDevice>(dev.id());

	return data;
}

template<>
TestGrpcDeviceInterface2 DeviceSerializerMethods<GRPCDevice>::deserialize<TestGrpcDeviceInterface2>(DeviceIdentifier &&devID, deserialization_t)
{
	return TestGrpcDeviceInterface2(std::move(devID));
}


class TestEngineGrpcClient
        : public EngineGrpcClient<TestEngineGrpcClient, TestEngineGRPCConfig, TestGrpcDeviceInterface1, TestGrpcDeviceInterface2>
{
    public:
        TestEngineGrpcClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
            : EngineGrpcClient(config, std::move(launcher))
        {}

		void initialize() override
        {
            this->sendInitCommand("test");
        }

		void shutdown() override
        {
            this->sendShutdownCommand("test");
        }
};

class TestEngineGrpcServer
    : public EngineGrpcServer
{
    public:
        template<class ...T>
        TestEngineGrpcServer(T &&...properties)
            : EngineGrpcServer(std::forward<T>(properties)...)
        {

        }

        virtual ~TestEngineGrpcServer() override = default;

        void initialize(const nlohmann::json &data, EngineGrpcServer::lock_t &) override
        {
            specialBehaviour();

            if(data.at("throw"))
            {
                throw std::runtime_error("Init failed");
            }
        }

        void shutdown(const nlohmann::json &data) override
        {
            specialBehaviour();

            if(data.at("throw"))
            {
                throw std::runtime_error("Shutdown failed");
            }
        }

        void timeoutOnNextCommand(int sleepTimeMs = 1500)
        {
            this->_sleepTimeMs = sleepTimeMs;
        }

        SimulationTime runLoopStep(const SimulationTime timeStep) override
        {
            specialBehaviour();

            _time += timeStep;

            return _time;
        }

        void resetEngineTime()
        {
            this->_time = SimulationTime::zero();
        }

    private:

        void specialBehaviour()
        {
            if(this->_sleepTimeMs != 0)
            {
                testSleep(this->_sleepTimeMs);
                this->_sleepTimeMs = 0;
            }
        }

        SimulationTime _time        = SimulationTime::zero();
        int            _sleepTimeMs = 0;
};

TEST(EngineGrpc, BASIC)
{
    // TODO This one has a linking issue, fix it!

    /*TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    ASSERT_EQ(client.getChannelStatus(), grpc_connectivity_state::GRPC_CHANNEL_IDLE);

    server.startServer();

    ASSERT_EQ(client.connect(), grpc_connectivity_state::GRPC_CHANNEL_READY);

    server.shutdownServer();*/
}

TEST(EngineGrpc, InitCommand)
{
    TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    nlohmann::json jsonMessage;
    jsonMessage["init"]    = true;
    jsonMessage["throw"]   = false;

    // The gRPC server isn't running, so the init command should fail

    ASSERT_THROW(client.sendInitCommand(jsonMessage), std::runtime_error);

    // Start the server and send the init command. It should succeed

    server.startServer();
    // TODO Investigate why this is needed. It seems to be caused by the previous call to sendInitCommand function
    testSleep(1500);
    client.sendInitCommand(jsonMessage);

    // Force the server to return an error from the rpc
    // Check if the client receives an error response on command handling failure

    jsonMessage["throw"] = true;
    ASSERT_THROW(client.sendInitCommand(jsonMessage), std::runtime_error);
}

TEST(EngineGrpc, InitCommandTimeout)
{
    TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config(nlohmann::json({{"EngineCommandTimeout", 0.0005}}));
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    nlohmann::json jsonMessage;
    jsonMessage["init"]    = true;
    jsonMessage["throw"]   = false;

    // Test init command timeout

    server.startServer();
    server.timeoutOnNextCommand();
    ASSERT_THROW(client.sendInitCommand(jsonMessage), std::runtime_error);
}

TEST(EngineGrpc, ShutdownCommand)
{
    TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    nlohmann::json jsonMessage;
    jsonMessage["shutdown"] = true;
    jsonMessage["throw"]    = false;

    // The gRPC server isn't running, so the shutdown command should fail

    ASSERT_THROW(client.sendShutdownCommand(jsonMessage), std::runtime_error);

    // Start the server and send the shutdown command. It should succeed

    server.startServer();
    // TODO Investigate why this is needed. It seems to be caused by the previous call to sendInitCommand function
    testSleep(1500);
    ASSERT_NO_THROW(client.sendShutdownCommand(jsonMessage));

    // Force the server to return an error from the rpc
    // Check if the client receives an error response on command handling failure

    jsonMessage["throw"] = true;
    ASSERT_THROW(client.sendShutdownCommand(jsonMessage), std::runtime_error);
}

TEST(EngineGrpc, ShutdownCommandTimeout)
{
    TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config(nlohmann::json({{"EngineCommandTimeout", 1}}));
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    nlohmann::json jsonMessage;
    jsonMessage["shutdown"] = true;
    jsonMessage["throw"]    = false;

    // Test shutdown command timeout

    server.startServer();
    server.timeoutOnNextCommand();
    ASSERT_THROW(client.sendShutdownCommand(jsonMessage), std::runtime_error);
}

static SimulationTime floatToSimulationTime(float time)
{
    return toSimulationTime<float, std::ratio<1>>(time);
}

TEST(EngineGrpc, RunLoopStepCommand)
{
    TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config;
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // The gRPC server isn't running, so the runLoopStep command should fail

    SimulationTime timeStep = floatToSimulationTime(0.1f);
    ASSERT_THROW(client.sendRunLoopStepCommand(timeStep), std::runtime_error);

    server.startServer();

    // Engine time should never be smaller than 0

    server.resetEngineTime();
    timeStep = floatToSimulationTime(-0.1f);
    ASSERT_THROW(client.sendRunLoopStepCommand(timeStep), std::runtime_error);

    // Normal loop execution, the command should return engine time

    server.resetEngineTime();
    timeStep = floatToSimulationTime(1.0f);
    ASSERT_NEAR(client.sendRunLoopStepCommand(timeStep).count(), timeStep.count(), 0.0001);

    // Try to go back in time. The client should raise an error when engine time is decreasing

    server.resetEngineTime();
    timeStep = floatToSimulationTime(2.0f);
    ASSERT_NO_THROW(client.sendRunLoopStepCommand(timeStep));
    timeStep = floatToSimulationTime(-1.0f);
    ASSERT_THROW(client.sendRunLoopStepCommand(timeStep), std::runtime_error);

    // TODO Add test for failure on server side
}

TEST(EngineGrpc, runLoopStepCommandTimeout)
{
    TestEngineGrpcServer               server;
    SimulationConfig::config_storage_t config(nlohmann::json({{"EngineCommandTimeout", 1}}));
    TestEngineGrpcClient               client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Test runLoopStep command timeout

    server.startServer();
    server.timeoutOnNextCommand();
    SimulationTime timeStep = floatToSimulationTime(2.0f);
    ASSERT_THROW(client.sendRunLoopStepCommand(timeStep), std::runtime_error);
}

TEST(EngineGrpc, RegisterDevices)
{
    TestEngineGrpcServer server;

    TestGrpcDeviceController dev1(DeviceIdentifier("dev1", "test", "test"));

    ASSERT_EQ(server.getNumRegisteredDevices(), 0);
    server.registerDevice("dev1", &dev1);
    ASSERT_EQ(server.getNumRegisteredDevices(), 1);
}

TEST(EngineGrpc, SetDeviceData)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    std::vector<DeviceInterface*> input_devices;

    const std::string deviceName = "a";
    const std::string deviceType = "test_type1";
    const std::string engineName = "c";

    client.engineName() = engineName;

    DeviceIdentifier         devId(deviceName, engineName, deviceType);
	TestGrpcDeviceInterface1 dev1((DeviceIdentifier(devId)));             // Client side
	TestGrpcDeviceController deviceController((DeviceIdentifier(devId))); // Server side

    server.registerDevice(deviceName, &deviceController);

    input_devices.push_back(&dev1);

    // The gRPC server isn't running, so the handleInputDevices command should fail

    ASSERT_THROW(client.handleInputDevices(input_devices), std::runtime_error);

    // Normal command execution

    server.startServer();
    testSleep(1500);
    client.handleInputDevices(input_devices);

	ASSERT_EQ(deviceController.getDeviceInformationCallback()->id().Name,       deviceName);
	ASSERT_EQ(deviceController.getDeviceInformationCallback()->id().Type,       deviceType);
	ASSERT_EQ(deviceController.getDeviceInformationCallback()->id().EngineName, engineName);

    // Test setting data on a device that wasn't registered in the engine server

    const std::string deviceName2 = "b";

    DeviceIdentifier         devId2(deviceName2, engineName, deviceType);
	TestGrpcDeviceInterface1 dev2((DeviceIdentifier(devId2)));

    input_devices.push_back(&dev2);

    ASSERT_THROW(client.handleInputDevices(input_devices), std::runtime_error);

    // TODO Add test for setData timeout
}

TEST(EngineGrpc, GetDeviceData)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Client sends a request to the server

    std::vector<DeviceInterface*> input_devices;

    const std::string deviceName = "a";
    const std::string deviceType = "test_type2";
    const std::string engineName = "c";

    client.engineName() = engineName;

    DeviceIdentifier         devId(deviceName, engineName, deviceType);
	TestGrpcDeviceInterface2 dev1((DeviceIdentifier(devId)));             // Client side
	TestGrpcDeviceController deviceController((DeviceIdentifier(devId))); // Server side

    server.registerDevice(deviceName, &deviceController);

    input_devices.push_back(&dev1);

    EngineInterface::device_identifiers_t deviceIdentifiers;
    deviceIdentifiers.insert(devId);

    // The gRPC server isn't running, so the getOutputDevices command should fail

    ASSERT_THROW(client.requestOutputDevices(deviceIdentifiers), std::runtime_error);

    // Normal command execution

    server.startServer();
    testSleep(1500);
    client.handleInputDevices(input_devices);

    const auto output = client.requestOutputDevices(deviceIdentifiers);

    ASSERT_EQ(output.size(), 1);
    ASSERT_EQ(output.at(0)->name(),       deviceName);
    ASSERT_EQ(output.at(0)->type(),       deviceType);
    ASSERT_EQ(output.at(0)->engineName(), engineName);

    // Test setting data on a device that wasn't registered in the engine server

    const std::string deviceName2 = "b";

    DeviceIdentifier         devId2(deviceName2, engineName, deviceType);
	TestGrpcDeviceInterface1 dev2((DeviceIdentifier(devId2)));

    deviceIdentifiers.insert(devId2);

    ASSERT_THROW(client.requestOutputDevices(deviceIdentifiers), std::runtime_error);

    // TODO Add test for getData timeout
}

TEST(EngineGrpc, GetDeviceData2)
{
    SimulationConfig::config_storage_t config;

    TestEngineGrpcServer server;
    TestEngineGrpcClient client(config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));

    // Client sends a request to the server

    std::vector<DeviceInterface*> input_devices;

    const std::string engineName = "c";

    const std::string deviceName1 = "a";
    const std::string deviceType1 = "test_type1";
    const std::string deviceName2 = "b";
    const std::string deviceType2 = "test_type2";

    client.engineName() = engineName;

    DeviceIdentifier         devId1(deviceName1, engineName, deviceType1);
    DeviceIdentifier         devId2(deviceName2, engineName, deviceType2);
	TestGrpcDeviceInterface1 dev1((DeviceIdentifier(devId1)));              // Client side
	TestGrpcDeviceInterface2 dev2((DeviceIdentifier(devId2)));              // Client side
	TestGrpcDeviceController deviceController1((DeviceIdentifier(devId1))); // Server side
	TestGrpcDeviceController deviceController2((DeviceIdentifier(devId2))); // Server side

    server.registerDevice(deviceName1, &deviceController1);
    server.registerDevice(deviceName2, &deviceController2);

    input_devices.push_back(&dev1);
    input_devices.push_back(&dev2);

    server.startServer();
    client.handleInputDevices(input_devices);

    EngineInterface::device_identifiers_t deviceIdentifiers;
    deviceIdentifiers.insert(devId1);
    deviceIdentifiers.insert(devId2);

    const auto output = client.requestOutputDevices(deviceIdentifiers);

    ASSERT_EQ(output.size(), 2);
    ASSERT_EQ(output.at(0)->engineName(), engineName);
    ASSERT_EQ(output.at(1)->engineName(), engineName);

    if(output.at(0)->type().compare(deviceType1) == 0)
    {
        ASSERT_EQ(output.at(0)->name(), deviceName1);
        ASSERT_EQ(output.at(1)->name(), deviceName2);
        ASSERT_EQ(output.at(1)->type(), deviceType2);
    }
    else
    {
        ASSERT_EQ(output.at(0)->name(), deviceName2);
        ASSERT_EQ(output.at(1)->name(), deviceName1);
        ASSERT_EQ(output.at(1)->type(), deviceType1);
    }
}

// EOF
