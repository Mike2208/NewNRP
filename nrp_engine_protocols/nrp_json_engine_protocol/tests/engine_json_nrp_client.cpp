#include <gtest/gtest.h>

#include "nrp_json_engine_protocol/config/engine_json_config.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_server.h"
#include "nrp_json_engine_protocol/nrp_client/engine_json_nrp_client.h"
#include "nrp_general_library/process_launchers/process_launcher_basic.h"

#include "tests/test_engine_json_device_controllers.h"

#include <future>
#include <restclient-cpp/restclient.h>

using namespace testing;

class TestEngineJSONConfig
        : public EngineJSONConfig<TestEngineJSONConfig, PropNames<> >
{
	public:
		static constexpr FixedString ConfigType = "TestEngineConfig";


		TestEngineJSONConfig(EngineConfigConst::config_storage_t &config)
		    : EngineJSONConfig(config)
		{}
};

class TestEngineJSONServer
        : public EngineJSONServer
{
	public:
	template<class ...T>
	TestEngineJSONServer(T &&...properties)
	    : EngineJSONServer(std::forward<T>(properties)...)
	{}

	virtual ~TestEngineJSONServer() override = default;

	float curTime = 0;

	float runLoopStep(float timeStep) override
	{
		if(timeStep < 0)
			throw std::invalid_argument("error");

		curTime += timeStep;

		return curTime;
	}

	nlohmann::json initialize(const nlohmann::json &data, EngineJSONServer::lock_t&) override
	{
		return nlohmann::json({{"status", "success"}, {"original", data}});
	}

	nlohmann::json shutdown(const nlohmann::json &data) override
	{
		return nlohmann::json({{"shutdown", "success"}, {"original", data}});
	}

	template<class EXCEPTION = std::exception>
	void startCatchException()
	{
		ASSERT_THROW(this->startServer(), EXCEPTION);
		this->_serverRunning = false;
	}
};

class TestEngineJSONNRPClient
        : public EngineJSONNRPClient<TestEngineJSONNRPClient, TestEngineJSONConfig, TestJSONDevice1, TestJSONDevice2, TestJSONDeviceThrow>
{
	public:
	template<class ...T>
	TestEngineJSONNRPClient(T &&...properties)
	    : EngineJSONNRPClient(std::forward<T>(properties)...)
	{}

	virtual ~TestEngineJSONNRPClient() override = default;

	RESULT initialize() override
	{
		auto retVal = this->sendInitCommand("initCommand");
		if (retVal["original"].get<std::string>().compare("initCommand") == 0)
			return SUCCESS;

		return ERROR;
	}

	RESULT shutdown() override
	{
		auto retVal = this->sendShutdownCommand("shutdownCommand");
		if (retVal["original"].get<std::string>().compare("shutdownCommand") == 0)
			return SUCCESS;

		return ERROR;
	}

	float curTime = 0;
};


TEST(EngineJSONNRPClientTest, ServerCalls)
{
	// Setup test server
	TestEngineJSONServer server("localhost:5463");

	const auto engineName = "engine1";
	const auto falseEngineName = "engineFalse";

	auto data = nlohmann::json({{"", {{"data", 1}}}});
	auto dev1 = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDevice1>(TestJSONDevice1::createID("device1", "engine_name_1"), data.begin());
	data = nlohmann::json({{"", {{"data", 2}}}});
	auto dev2 = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDevice2>(TestJSONDevice2::createID("device2", "engine_name_2"), data.begin());
	data = nlohmann::json({{"", {{"data", -1}}}});
	auto devThrow = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDeviceThrow>(TestJSONDeviceThrow::createID("deviceThrow", "engine_throw"), data.begin());

	dev1.setEngineName(engineName);
	dev2.setEngineName(engineName);
	devThrow.setEngineName(falseEngineName);

	dev1.data() = 3;
	dev2.data() = 5;

	// Register device controllers
	auto dev1Ctrl = TestJSONDevice1Controller(DeviceIdentifier(dev1.id()));
	server.registerDevice(dev1.name(), &dev1Ctrl);
	auto dev2Ctrl = TestJSONDevice2Controller(DeviceIdentifier(dev2.id()));
	server.registerDevice(dev2.name(), &dev2Ctrl);

	// Check timeout if no server is running
	SimulationConfig::config_storage_t config;
	TestEngineJSONNRPClient fakeClient("localhost:" + std::to_string(server.serverPort()), config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));
	ASSERT_THROW(fakeClient.initialize(), NRPExceptionNonRecoverable);

	// Start server, test init
	server.startServerAsync();
	TestEngineJSONNRPClient client("localhost:" + std::to_string(server.serverPort()), config, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()));
	client.engineName() = engineName;
	ASSERT_EQ(client.initialize(), TestEngineJSONNRPClient::SUCCESS);

	ASSERT_EQ(client.runLoopStep(10), TestEngineJSONNRPClient::SUCCESS);
	ASSERT_EQ(client.waitForStepCompletion(10), TestEngineJSONNRPClient::SUCCESS);

	ASSERT_EQ(client.getEngineTime(), server.curTime);

	// Test device retrieval
	TestEngineJSONNRPClient::device_identifiers_t devIDs({dev1.id(), dev2.id(), devThrow.id()});
	auto devices = client.requestOutputDevices(devIDs);

	// Only two devices (dev1, dev2) should be retrieved, as they are associated with the correct EngineName
	ASSERT_EQ(devices.size(), 2);

	// Assign correct devices, order can be arbitrary
	const DeviceInterface *retDev1BasePtr = nullptr, *retDev2BasePtr = nullptr;
	if(devices.begin()->get()->id() == dev1.id())
	{
		retDev1BasePtr = devices.begin()->get();
		retDev2BasePtr = (++devices.begin())->get();
	}
	else if(devices.begin()->get()->id() == dev2.id())
	{
		retDev2BasePtr = devices.begin()->get();
		retDev1BasePtr = (++devices.begin())->get();
	}

	ASSERT_NE(retDev1BasePtr, nullptr);
	ASSERT_NE(retDev2BasePtr, nullptr);

	const auto &retDev1 = dynamic_cast<const TestJSONDevice1&>(*retDev1BasePtr);
	const auto &retDev2 = dynamic_cast<const TestJSONDevice2&>(*retDev2BasePtr);

	ASSERT_EQ(retDev1.data(), dev1Ctrl.data().data());
	ASSERT_EQ(retDev2.data(), dev2Ctrl.data().data());

	auto inputDev1 = dev1;
	auto inputDev2 = dev2;
	auto inputDevThrow = devThrow;
	dev1.data() = 6;
	dev2.data() = -1;

	// Test device sending
	TestEngineJSONNRPClient::device_inputs_t inputs;
	inputs.push_back(&inputDev1);
	inputs.push_back(&inputDev2);
	inputs.push_back(&inputDevThrow);
	ASSERT_EQ(client.handleInputDevices(inputs), TestEngineJSONNRPClient::SUCCESS);

	ASSERT_EQ(inputDev1.data(), dev1Ctrl.data().data());
	ASSERT_EQ(inputDev2.data(), dev2Ctrl.data().data());
}
