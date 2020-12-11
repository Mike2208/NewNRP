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
#include <fstream>

#include "nrp_general_library/process_launchers/process_launcher_basic.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_simulation/simulation/simulation_loop.h"
#include "tests/test_env_cmake.h"

#define DEBUG_NO_CREATE_ENGINE_LAUNCHER_FCN
#include "nrp_nest_json_engine/nrp_client/nest_engine_json_nrp_client.h"
#include "nrp_gazebo_grpc_engine/nrp_client/gazebo_engine_grpc_nrp_client.h"


using namespace testing;

TEST(SimulationLoopTest, InitTFManager)
{
	auto simConfigFile = std::fstream(TEST_SIM_SIMPLE_CONFIG_FILE, std::ios::in);
	nlohmann::json simConfig = nlohmann::json::parse(simConfigFile);

	const char *procName = "test";
	PythonInterpreterState pyState(1, const_cast<char**>(&procName));

	SimulationConfigSharedPtr config(new SimulationConfig(simConfig));
	ASSERT_NO_THROW(auto tfManager = SimulationLoop::initTFManager(config, {}));
}

TEST(SimulationLoopTest, Constructor)
{
	auto simConfigFile = std::fstream(TEST_SIM_SIMPLE_CONFIG_FILE, std::ios::in);
	nlohmann::json simConfig = nlohmann::json::parse(simConfigFile);

	const char *procName = "test";
	PythonInterpreterState pyState(1, const_cast<char**>(&procName));

	SimulationConfigSharedPtr config(new SimulationConfig(simConfig));
	config->engineConfigs().resize(2);

	EngineInterfaceSharedPtr brain(NestEngineJSONLauncher().launchEngine(config->engineConfigs().at(0), ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	config->engineConfigs().at(1).Data = nlohmann::json({{GazeboGrpcConfigConst::GazeboWorldFile, ""}});
	EngineInterfaceSharedPtr physics(GazeboEngineGrpcLauncher().launchEngine(config->engineConfigs().at(1), ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NO_THROW(SimulationLoop simLoop(config, {brain, physics}));
}

TEST(SimulationLoopTest, RunLoop)
{
	auto simConfigFile = std::fstream(TEST_SIM_SIMPLE_CONFIG_FILE, std::ios::in);
	nlohmann::json simConfig = nlohmann::json::parse(simConfigFile);

	const char *procName = "test";
	PythonInterpreterState pyState(1, const_cast<char**>(&procName));

	const SimulationTime timestep(10);
	const float timeStepFloat = 0.01f;

	SimulationConfigSharedPtr config(new SimulationConfig(simConfig));
	config->engineConfigs().resize(2);

	{
		NestConfig nestCfg(config->engineConfigs().at(0));

		config->engineConfigs().at(1).Data = nlohmann::json({{GazeboGrpcConfigConst::GazeboWorldFile, ""}});
		GazeboGrpcConfig gazeboCfg(config->engineConfigs().at(1));

		nestCfg.nestInitFileName() = TEST_NEST_SIM_FILE;
		nestCfg.engineTimestep() = timeStepFloat;

		gazeboCfg.gazeboWorldFile() = TEST_GAZEBO_WORLD_FILE;
		gazeboCfg.engineTimestep() = timeStepFloat;

		config->engineConfigs().at(0).Data = nestCfg.writeConfig();
		config->engineConfigs().at(1).Data = gazeboCfg.writeConfig();
	}

	EngineInterfaceSharedPtr brain(NestEngineJSONLauncher().launchEngine(config->engineConfigs().at(0), ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));
	EngineInterfaceSharedPtr physics(GazeboEngineGrpcLauncher().launchEngine(config->engineConfigs().at(1), ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	SimulationLoop simLoop(config, {brain, physics});

	ASSERT_NO_THROW(simLoop.initLoop());

	ASSERT_EQ(simLoop.getSimTime(), SimulationTime::zero());
	ASSERT_NO_THROW(simLoop.runLoop(timestep));
	ASSERT_EQ(simLoop.getSimTime(), timestep);
	ASSERT_NO_THROW(simLoop.runLoop(timestep));
	ASSERT_EQ(simLoop.getSimTime(), timestep+timestep);
}
