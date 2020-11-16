#include <gtest/gtest.h>

#include "nrp_general_library/device_interface/devices/python_object_device_interface.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_python_json_engine/engine_server/python_json_server.h"
#include "tests/test_env_cmake.h"

#include <boost/python.hpp>
#include <future>
#include <restclient-cpp/restclient.h>

namespace python = boost::python;

TEST(TestPythonJSONServer, TestFunc)
{
	std::string argvDat = "TestProg";
	char *argv = &argvDat[0];
	PythonInterpreterState pyState(1, &argv);

	python::dict pyGlobals = python::dict(python::import("__main__").attr("__dict__"));
	python::object pyLocals;

	auto cfg = PythonConfig(nlohmann::json());
	cfg.pythonFileName() = TEST_PYTHON_DEVICE_FILE_NAME;
	cfg.engineServerAddress() = "localhost:5432";

	PythonJSONServer server(cfg.engineServerAddress(), pyGlobals, pyLocals);

	// Test Init
	nlohmann::json req = nlohmann::json({{PythonConfig::ConfigType.m_data, cfg.writeConfig()}});
	pyState.allowThreads();

	EngineJSONServer::mutex_t fakeMutex;
	EngineJSONServer::lock_t fakeLock(fakeMutex);
	nlohmann::json respParse = server.initialize(req, fakeLock);

	const auto execResult = respParse[PythonConfig::InitFileExecStatus.data()].get<bool>();
	ASSERT_EQ(execResult, true);
	ASSERT_EQ(server.initRunFlag(), true);

	// Test runStep REST call
	const float timeStep = 1;
	ASSERT_EQ(server.runLoopStep(timeStep), timeStep);

	// Test getDevice REST call EngineServerGetDevicesRoute
	server.startServerAsync();

	pyState.endAllowThreads();
	req = nlohmann::json({{"device1", 0}});
	auto resp = RestClient::post(cfg.engineServerAddress() + "/" + EngineJSONConfigConst::EngineServerGetDevicesRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), req.dump());
	respParse = nlohmann::json::parse(resp.body);

	// Test Python Device data deserialization
	PythonObjectDeviceInterface dev = JSONDeviceConversionMechanism<>::deserialize<PythonObjectDeviceInterface>(respParse.begin());

	dev.PythonObjectDeviceInterface::data().Data = python::dict(dev.PythonObjectDeviceInterface::data().deserialize());

	// TODO: Test Sending data

	ASSERT_EQ(respParse["device1"][PythonObjectDeviceInterfaceConst::Object.m_data].size(), python::len(dev.data().Data));
	float recTime = python::extract<float>(dev.data().Data["time"]);
	ASSERT_EQ(0, recTime);

	pyState.endAllowThreads();
	server.shutdownServer();
}

TEST(TestPythonJSONServer, TestInitError)
{
	std::string argvDat = "TestProg";
	char *argv = &argvDat[0];
	PythonInterpreterState pyState(1, &argv);

	auto pyGlobals = python::dict(python::import("__main__").attr("__dict__"));
	python::object pyLocals;

	auto cfg = PythonConfig(nlohmann::json());
	cfg.pythonFileName() = TEST_PYTHON_INIT_ERROR_FILE_NAME;
	cfg.engineServerAddress() = "localhost:5432";

	PythonJSONServer server(cfg.engineServerAddress(), pyGlobals, pyLocals);

	nlohmann::json req = nlohmann::json({{PythonConfig::ConfigType.m_data, cfg.writeConfig()}});
	pyState.allowThreads();

	EngineJSONServer::mutex_t fakeMutex;
	EngineJSONServer::lock_t fakeLock(fakeMutex);
	nlohmann::json respParse = server.initialize(req, fakeLock);

	pyState.endAllowThreads();
}
