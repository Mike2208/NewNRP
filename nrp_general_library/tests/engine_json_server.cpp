#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_server.h"

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

	TestJSONDeviceInterface1 dev1("device1", nlohmann::json({"data", 1}));
	TestJSONDeviceInterface2 dev2("device2", nlohmann::json({"data", 2}));
	TestJSONDeviceInterfaceThrow devThrow("deviceThrow", nlohmann::json({"data", -1}));

	// Register devices
	server.registerDevice(dev1.name(), &dev1);
	server.registerDevice(dev2.name(), &dev2);
	server.registerDevice(devThrow.name(), &devThrow);

	// Set Data
	auto retData = server.setDeviceData(nlohmann::json());
	ASSERT_TRUE(retData.empty());

	retData = server.setDeviceData(nlohmann::json({{"fakeDevice", {}}}));
	ASSERT_STREQ(retData.find("fakeDevice")->get<std::string>().data(), "");
	ASSERT_EQ(retData.size(), 1);

	nlohmann::json data;
	data.update(dev1.convertDeviceToJSON(dev1));
	data.update(dev2.convertDeviceToJSON(dev2));
	retData = server.setDeviceData(data);
	ASSERT_STREQ(retData["device1"]["handledData1"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface1::TypeName.data());
	ASSERT_STREQ(retData["device2"]["handledData2"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface2::TypeName.data());
	ASSERT_EQ(retData.size(), 2);

	data.clear();
	data.update(devThrow.convertDeviceToJSON(devThrow));
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
	ASSERT_STREQ(retData["device1"]["retrievedData1"]["device1"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface1::TypeName.data());
	ASSERT_STREQ(retData["device2"]["retrievedData2"]["device2"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface2::TypeName.data());
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

	TestJSONDeviceInterface1 dev1("device1", nlohmann::json({"data", 1}));
	TestJSONDeviceInterface2 dev2("device2", nlohmann::json({"data", 2}));
	TestJSONDeviceInterfaceThrow devThrow("deviceThrow", nlohmann::json({"data", -1}));

	// Register devices
	server.registerDevice(dev1.name(), &dev1);
	server.registerDevice(dev2.name(), &dev2);
	server.registerDevice(devThrow.name(), &devThrow);

	ASSERT_FALSE(server.isServerRunning());
	server.startServerAsync();
	ASSERT_TRUE(server.isServerRunning());

	// Init command
	nlohmann::json data;
	data.emplace("init", nlohmann::json());
	auto resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerInitializeRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	nlohmann::json retData = nlohmann::json::parse(resp.body);
	ASSERT_STREQ(retData["status"].get<std::string>().data(), "success");

	// Run step command
	data.clear();
	data[EngineJSONConfigConst::EngineTimeStepName.data()] = 1;
	resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerRunLoopStepRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	ASSERT_EQ(server.curTime, 1);

	// Run get server command
	data.clear();
	data.update(dcm_t::serializeID(dev1.id()));
	data.update(dcm_t::serializeID(dev2.id()));
	resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerGetDevicesRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	retData = nlohmann::json::parse(resp.body);
	ASSERT_STREQ(retData["device1"]["retrievedData1"]["device1"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface1::TypeName.data());
	ASSERT_STREQ(retData["device2"]["retrievedData2"]["device2"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface2::TypeName.data());
	ASSERT_EQ(retData.size(), 2);

	// Run set server command
	data.clear();
	data.update(dev1.convertDeviceToJSON(dev1));
	data.update(dev2.convertDeviceToJSON(dev2));
	resp = RestClient::post(address + "/" + EngineJSONConfigConst::EngineServerSetDevicesRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
	retData = nlohmann::json::parse(resp.body);
	ASSERT_STREQ(retData["device1"]["handledData1"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface1::TypeName.data());
	ASSERT_STREQ(retData["device2"]["handledData2"][dcm_t::JSONTypeID.data()].get<std::string>().data(), TestJSONDeviceInterface2::TypeName.data());
	ASSERT_EQ(retData.size(), 2);
}
