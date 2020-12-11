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

#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_simulation/simulation/simulation_manager.h"
#include "nrp_simulation/config/cmake_conf.h"
#include "tests/test_env_cmake.h"

#define DEBUG_NO_CREATE_ENGINE_LAUNCHER_FCN
#include "nrp_nest_json_engine/nrp_client/nest_engine_json_nrp_client.h"
#include "nrp_gazebo_grpc_engine/nrp_client/gazebo_engine_grpc_nrp_client.h"

using namespace testing;

std::vector<const char*> createStartParamPtr(const std::vector<std::string> &startParamDat)
{
	std::vector<const char*> retVal;
	retVal.reserve(startParamDat.size());

	for(const auto &param : startParamDat)
	{
		retVal.push_back(param.data());
	}

	return retVal;
}

TEST(SimulationManagerTest, OptParser)
{
	auto optParser(SimulationParams::createStartParamParser());

	const char *pPyArgv = "simtest";
	PythonInterpreterState pyInterp(1, const_cast<char**>(&pPyArgv));

	std::vector<std::string> startParamDat;
	std::vector<const char*> startParams;

	// Test valid parameters
	startParamDat = {"nrp_server",
	                std::string("-") + SimulationParams::ParamHelp.data(),
	                std::string("-") + SimulationParams::ParamSimCfgFile.data(), "cfgFile.json"};

	startParams = createStartParamPtr(startParamDat);

	int argc = static_cast<int>(startParams.size());
	char **argv = const_cast<char**>(startParams.data());

	ASSERT_NO_THROW(optParser.parse(argc, argv));

	// Test invalid options
	startParams = {"nrp_server", "-fdsafdaf"};
	argc = static_cast<int>(startParams.size());
	argv = const_cast<char**>(startParams.data());

	ASSERT_THROW(optParser.parse(argc, argv), cxxopts::OptionParseException);

	startParamDat = {"nrp_server", std::string("-") + SimulationParams::ParamSimCfgFile.data()};
	startParams = createStartParamPtr(startParamDat);

	argc = static_cast<int>(startParams.size());
	argv = const_cast<char**>(startParams.data());

	ASSERT_THROW(optParser.parse(argc, argv), cxxopts::OptionParseException);
}

TEST(SimulationManagerTest, SimulationManagerSetup)
{
	auto optParser(SimulationParams::createStartParamParser());

	const char *pPyArgv = "simtest";
	PythonInterpreterState pyInterp(1, const_cast<char**>(&pPyArgv));

	// Test no simulation file passed
	std::vector<std::string> startParamDat;
	std::vector<const char*> startParams = {"nrp_server"};
	int argc = static_cast<int>(startParams.size());
	char **argv = const_cast<char**>(startParams.data());

	{
		auto startParamVals(optParser.parse(argc, argv));
		SimulationManager manager = SimulationManager::createFromParams(startParamVals);

		ASSERT_EQ(manager.simulationConfig(), nullptr);
	}

	// Test non-existent file
	startParamDat = {"nrp_server",
	               std::string("-") + SimulationParams::ParamSimCfgFile.data(), "noFile.json"};
	startParams = createStartParamPtr(startParamDat);

	argc = static_cast<int>(startParams.size());
	argv = const_cast<char**>(startParams.data());

	{
		auto startParamVals(optParser.parse(argc, argv));
		ASSERT_THROW(SimulationManager manager = SimulationManager::createFromParams(startParamVals), std::invalid_argument);
	}

	// Test invalid JSON config file
	startParamDat = {"nrp_server",
	                 std::string("-") + SimulationParams::ParamSimCfgFile.data(), TEST_INVALID_JSON_FILE};
	startParams = createStartParamPtr(startParamDat);

	argc = static_cast<int>(startParams.size());
	argv = const_cast<char**>(startParams.data());

	{
		auto startParamVals(optParser.parse(argc, argv));
		ASSERT_THROW(SimulationManager manager = SimulationManager::createFromParams(startParamVals), std::invalid_argument);
	}

	// Test valid JSON config file
	startParamDat = {"nrp_server",
	                 std::string("-") + SimulationParams::ParamSimCfgFile.data(), TEST_SIM_SIMPLE_CONFIG_FILE};
	startParams = createStartParamPtr(startParamDat);

	argc = static_cast<int>(startParams.size());
	argv = const_cast<char**>(startParams.data());

	{
		auto startParamVals(optParser.parse(argc, argv));
		SimulationManager manager = SimulationManager::createFromParams(startParamVals);

		ASSERT_NE(manager.simulationConfig(), nullptr);
	}
}

TEST(SimulationManagerTest, SimulationManagerLoop)
{
	auto optParser(SimulationParams::createStartParamParser());

	const char *pPyArgv = "simtest";
	PythonInterpreterState pyInterp(1, const_cast<char**>(&pPyArgv));

	std::vector<std::string> startParamDat = {"nrp_server",
	                                          std::string("-") + SimulationParams::ParamSimCfgFile.data(), TEST_SIM_CONFIG_FILE};
	std::vector<const char*> startParams = createStartParamPtr(startParamDat);

	auto argc = static_cast<int>(startParams.size());
	auto argv = const_cast<char**>(startParams.data());

	auto startParamVals(optParser.parse(argc, argv));
	SimulationManager manager = SimulationManager::createFromParams(startParamVals);

	EngineLauncherManagerSharedPtr engines(new EngineLauncherManager());
	MainProcessLauncherManagerSharedPtr processManager(new MainProcessLauncherManager());

	// Create brain and physics managers

	// Exception if required brain/physics engine launcher is not added
	auto simLock = manager.acquireSimLock();
	ASSERT_THROW(manager.initSimulationLoop(engines, processManager, simLock), std::invalid_argument);

	// Add launchers
	engines->registerLauncher(EngineLauncherInterfaceSharedPtr(new GazeboEngineGrpcLauncher()));
	engines->registerLauncher(EngineLauncherInterfaceSharedPtr(new NestEngineJSONLauncher()));

	manager.initSimulationLoop(engines, processManager, simLock);

	ASSERT_TRUE(manager.runSimulation(SimulationTime(1000000), simLock));
}
