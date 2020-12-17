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

#include <gtest/gtest.h>

#include "nrp_json_engine_protocol/config/engine_json_config.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_server.h"

#include "tests/test_engine_json_device_controllers.h"

#include <future>
#include <restclient-cpp/restclient.h>

using namespace testing;

using dcm_t = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator>;

class TestEngineJSONServer
        : public EngineJSONServer
{
	public:
	template<class ...T>
	TestEngineJSONServer(T &&...properties)
	    : EngineJSONServer(std::forward<T>(properties)...)
	{}

	virtual ~TestEngineJSONServer() override = default;

	SimulationTime curTime = SimulationTime::zero();

	SimulationTime runLoopStep(SimulationTime timeStep) override
	{
		if(timeStep < SimulationTime::zero())
			throw std::invalid_argument("error");

		curTime += timeStep;

		return curTime;
	}

	nlohmann::json initialize(const nlohmann::json &data, EngineJSONServer::lock_t&) override
	{
		return nlohmann::json({{"status", "success"}, {"original", data}});
	}

	// TODO: Test shutdown
	nlohmann::json shutdown(const nlohmann::json &data) override
	{
		return nlohmann::json({{"status", "shutdown"}, {"original", data}});
	}

	template<class EXCEPTION = std::exception>
	void startCatchException()
	{
		ASSERT_THROW(this->startServer(), EXCEPTION);
		this->_serverRunning = false;
	}
};

TEST(EngineJSONServerTest, Functions)
{
	TestEngineJSONServer server;

	auto data = nlohmann::json({{"", {{"data", 1}}}});
	auto dev1 = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDevice1>(TestJSONDevice1::createID("device1", "engine_name_1"), data.begin());
	data = nlohmann::json({{"", {{"data", 2}}}});
	auto dev2 = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDevice2>(TestJSONDevice2::createID("device2", "engine_name_2"), data.begin());
	data = nlohmann::json({{"", {{"data", -1}}}});
	auto devThrow = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDeviceThrow>(TestJSONDeviceThrow::createID("deviceThrow", "engine_throw"), data.begin());

	// Register device controllers
	auto dev1Ctrl = TestJSONDevice1Controller(DeviceIdentifier(dev1.id()));
	server.registerDevice(dev1.name(), &dev1Ctrl);
	auto dev2Ctrl = TestJSONDevice2Controller(DeviceIdentifier(dev2.id()));
	server.registerDevice(dev2.name(), &dev2Ctrl);
	auto devThrowCtrl = TestJSONDeviceThrowController(DeviceIdentifier(devThrow.id()));
	server.registerDevice(devThrow.name(), &devThrowCtrl);

	// Set Data
	auto retData = server.setDeviceData(nlohmann::json());
	ASSERT_TRUE(retData.empty());

	retData = server.setDeviceData(nlohmann::json({{"fakeDevice", {}}}));
	ASSERT_STREQ(retData.find("fakeDevice")->get<std::string>().data(), "");
	ASSERT_EQ(retData.size(), 1);

	data.clear();
	data.update(DeviceSerializerMethods<nlohmann::json>::serialize(dev1));
	data.update(DeviceSerializerMethods<nlohmann::json>::serialize(dev2));
	retData = server.setDeviceData(data);
	ASSERT_EQ(dev1Ctrl.data().data(), dev1.data());
	ASSERT_EQ(dev2Ctrl.data().data(), dev2.data());
	ASSERT_EQ(retData.size(), 2);

	data.clear();
	data.update(DeviceSerializerMethods<nlohmann::json>::serialize(devThrow));
	ASSERT_THROW(server.setDeviceData(data), NRPExceptionNonRecoverable);

	// Get Data
	retData = server.getDeviceData(nlohmann::json());
	ASSERT_TRUE(retData.empty());

	retData = server.getDeviceData(nlohmann::json({{"fakeDevice", {}}}));
	ASSERT_TRUE(retData.find("fakeDevice")->empty());
	ASSERT_EQ(retData.size(), 1);

	data = nlohmann::json();
	data.update(dcm_t::serializeID(dev1.id()));
	data.update(dcm_t::serializeID(dev2.id()));
	retData = server.getDeviceData(data);
	ASSERT_STREQ(retData["device1"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDevice1::TypeName.data());
	ASSERT_STREQ(retData["device2"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDevice2::TypeName.data());
	ASSERT_EQ(retData.size(), 2);

	data = dcm_t::serializeID(devThrow.id());
	ASSERT_THROW(server.getDeviceData(data), std::invalid_argument);

	// Clear devices
	server.clearRegisteredDevices();
	ASSERT_EQ(server._devicesControllers.size(), 0);
}

TEST(EngineJSONServerTest, HttpRequests)
{
	const std::string address = "localhost:5432";
	TestEngineJSONServer server(address);

	auto data = nlohmann::json({{"", {{"data", 1}}}});
	auto dev1 = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDevice1>(TestJSONDevice1::createID("device1", "engine_name_1"), data.begin());
	data = nlohmann::json({{"", {{"data", 2}}}});
	auto dev2 = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDevice2>(TestJSONDevice2::createID("device2", "engine_name_2"), data.begin());
	data = nlohmann::json({{"", {{"data", -1}}}});
	auto devThrow = DeviceSerializerMethods<nlohmann::json>::deserialize<TestJSONDeviceThrow>(TestJSONDeviceThrow::createID("deviceThrow", "engine_throw"), data.begin());

	// Register device controllers
	auto dev1Ctrl = TestJSONDevice1Controller(DeviceIdentifier(dev1.id()));
	server.registerDevice(dev1.name(), &dev1Ctrl);
	auto dev2Ctrl = TestJSONDevice2Controller(DeviceIdentifier(dev2.id()));
	server.registerDevice(dev2.name(), &dev2Ctrl);
	auto devThrowCtrl = TestJSONDeviceThrowController(DeviceIdentifier(devThrow.id()));
	server.registerDevice(devThrow.name(), &devThrowCtrl);

	ASSERT_FALSE(server.isServerRunning());
	server.startServerAsync();
	ASSERT_TRUE(server.isServerRunning());

	// Init command
	data.clear();
	data.emplace("init", nlohmann::json());
	auto resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerInitializeRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	nlohmann::json retData = nlohmann::json::parse(resp.body);
	ASSERT_STREQ(retData["status"].get<std::string>().data(), "success");

	SimulationTime runTime = toSimulationTime<int, std::milli>(1);
	// Run step command
	data.clear();
	data[EngineJSONConfigConst::EngineTimeStepName.data()] = runTime.count();
	resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerRunLoopStepRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	ASSERT_EQ(server.curTime, runTime);

	// Run get server command
	data.clear();
	data.update(dcm_t::serializeID(dev1.id()));
	data.update(dcm_t::serializeID(dev2.id()));
	resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerGetDevicesRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	retData = nlohmann::json::parse(resp.body);
	ASSERT_STREQ(retData["device1"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDevice1::TypeName.data());
	ASSERT_STREQ(retData["device2"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDevice2::TypeName.data());
	ASSERT_EQ(retData.size(), 2);

	// Run set server command
	data.clear();
	data.update(DeviceSerializerMethods<nlohmann::json>::serialize(dev1));
	data.update(DeviceSerializerMethods<nlohmann::json>::serialize(dev2));
	resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerSetDevicesRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	retData = nlohmann::json::parse(resp.body);
	ASSERT_EQ(dev1Ctrl.data().data(), dev1.data());
	ASSERT_EQ(dev2Ctrl.data().data(), dev2.data());
	ASSERT_EQ(retData.size(), 2);
}
