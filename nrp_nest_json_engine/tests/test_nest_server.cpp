#include <gtest/gtest.h>

#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_nest_json_engine/engine_server/nest_json_server.h"
#include "tests/test_env_cmake.h"

#include <boost/python.hpp>
#include <future>
#include <restclient-cpp/restclient.h>

namespace python = boost::python;

TEST(TestNestJSONServer, TestFunc)
{
	std::string argvDat = "TestProg";
	char *argv = &argvDat[0];
	PythonInterpreterState pyState(1, &argv);

	python::dict pyGlobals = python::dict(python::import("__main__").attr("__dict__"));
	python::object pyLocals;

	auto cfg = NestConfig(nlohmann::json());
	cfg.nestInitFileName() = TEST_NEST_DEVICE_FILE_NAME;
	cfg.engineServerAddress() = "localhost:5432";

	NestJSONServer server(cfg.engineServerAddress(), pyGlobals, pyLocals);

	// Test Init
	nlohmann::json req = nlohmann::json({{NestConfig::ConfigType.m_data, cfg.writeConfig()}});
	pyState.allowThreads();

	EngineJSONServer::mutex_t fakeMutex;
	EngineJSONServer::lock_t fakeLock(fakeMutex);
	nlohmann::json respParse = server.initialize(req, fakeLock);

	const auto execResult = respParse[NestConfig::InitFileExecStatus.data()].get<bool>();
	ASSERT_EQ(execResult, true);
	ASSERT_EQ(server.initRunFlag(), true);

	// Test runStep REST call
	SimulationTime timeStep(1);
	ASSERT_EQ(server.runLoopStep(timeStep), timeStep);

	// Test getDevice REST call EngineServerGetDevicesRoute
	server.startServerAsync();

	req = nlohmann::json({{"voltmeter", 0}});
	auto resp = RestClient::post(cfg.engineServerAddress() + "/" + EngineJSONConfigConst::EngineServerGetDevicesRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), req.dump());
	respParse = nlohmann::json::parse(resp.body);

	const std::string jsonDat = respParse["voltmeter"][PyObjectDeviceConst::Object.m_data]["element_type"].get<std::string>();
	ASSERT_STREQ(jsonDat.data(), "recorder");

	pyState.endAllowThreads();

	// Test Nest Device data deserialization
	NestDeviceInterface dev = JSONDeviceConversionMechanism<>::deserialize<NestDeviceInterface>(respParse.begin());

	dev.PyObjectDevice::data() = python::dict(dev.PyObjectDevice::data().deserialize(""));

	// TODO: Test Sending data

	ASSERT_EQ(respParse["voltmeter"][PyObjectDeviceConst::Object.m_data].size(), python::len(dev.data()));
	ASSERT_EQ(jsonDat, std::string(python::extract<std::string>(dev.data()["element_type"])));

	server.shutdownServer();
}

TEST(TestNestJSONServer, TestInitError)
{
	std::string argvDat = "TestProg";
	char *argv = &argvDat[0];
	PythonInterpreterState pyState(1, &argv);

	auto pyGlobals = python::dict(python::import("__main__").attr("__dict__"));
	python::object pyLocals;

	auto cfg = NestConfig(nlohmann::json());
	cfg.nestInitFileName() = TEST_NEST_INIT_ERROR_NAME;
	cfg.engineServerAddress() = "localhost:5432";

	NestJSONServer server(cfg.engineServerAddress(), pyGlobals, pyLocals);

	nlohmann::json req = nlohmann::json({{NestConfig::ConfigType.m_data, cfg.writeConfig()}});
	pyState.allowThreads();

	EngineJSONServer::mutex_t fakeMutex;
	EngineJSONServer::lock_t fakeLock(fakeMutex);
	nlohmann::json respParse = server.initialize(req, fakeLock);

	pyState.endAllowThreads();
}
