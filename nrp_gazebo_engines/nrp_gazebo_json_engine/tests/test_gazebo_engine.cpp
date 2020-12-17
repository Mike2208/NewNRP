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

#include "nrp_gazebo_json_engine/config/gazebo_json_config.h"
#include "nrp_gazebo_json_engine/config/cmake_constants.h"
#include "nrp_gazebo_devices/physics_camera.h"
#include "nrp_gazebo_devices/physics_joint.h"
#include "nrp_gazebo_devices/physics_link.h"
#include "nrp_gazebo_json_engine/nrp_client/gazebo_engine_json_nrp_client.h"
#include "nrp_general_library/process_launchers/process_launcher_basic.h"

#include "tests/test_env_cmake.h"

#include <fstream>

TEST(TestGazeboEngine, Start)
{
	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboJSONConfigConst::GazeboWorldFile, ""}});

	GazeboJSONConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_EMPTY_WORLD_FILE;
	conf.maxWorldLoadTime() = 1;

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	std::cout << confHolder.Data.dump(4, ' ') << std::endl;

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<GazeboEngineJSONNRPClient>::shared_ptr engine = std::dynamic_pointer_cast<GazeboEngineJSONNRPClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_ANY_THROW(engine->initialize());
}

TEST(TestGazeboEngine, WorldPlugin)
{
	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboJSONConfigConst::GazeboWorldFile, ""}});

	GazeboJSONConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_WORLD_PLUGIN_FILE;

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<GazeboEngineJSONNRPClient>::shared_ptr engine = std::dynamic_pointer_cast<GazeboEngineJSONNRPClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_NO_THROW(engine->initialize());
	ASSERT_NO_THROW(engine->runLoopStep(toSimulationTime<int, std::milli>(100)));
	ASSERT_NO_THROW(engine->waitForStepCompletion(5.0f));
}

TEST(TestGazeboEngine, CameraPlugin)
{
	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboJSONConfigConst::GazeboWorldFile, ""}});

	GazeboJSONConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_CAMERA_PLUGIN_FILE;

	conf.userProcEnvParams().push_back("GAZEBO_MODEL_PATH=" TEST_GAZEBO_MODELS_DIR ":$GAZEBO_MODEL_PATH");

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<GazeboEngineJSONNRPClient>::shared_ptr engine = std::dynamic_pointer_cast<GazeboEngineJSONNRPClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_NO_THROW(engine->initialize());

	auto devices = engine->requestOutputDevices({DeviceIdentifier("nrp_camera::camera", conf.engineName(), PhysicsCamera::TypeName.data())});
	ASSERT_EQ(devices.size(), 1);

	const PhysicsCamera &camDat = dynamic_cast<const PhysicsCamera&>(*(devices[0]));
//	std::cout << "Image Height:" << camDat.imageHeight() << std::endl;
//	std::cout << "Image Width:" << camDat.imageWidth() << std::endl;
//	std::cout << "Image Pixel Size:" << std::to_string(camDat.imagePixelSize()) << std::endl;
//	std::cout << "Image Pixel Num:" << camDat.imageData().size() << std::endl;

	ASSERT_EQ(camDat.imageHeight(), 240);
	ASSERT_EQ(camDat.imageWidth(),  320);
	ASSERT_EQ(camDat.imagePixelSize(),  3);
	ASSERT_EQ(camDat.imageData().size(), 320*240*3);

	// Check if cam data is correct (WARNING: Data might be incorrect due to non-determinism. Should be checked)
	//std::fstream compareCamDat(TEST_CAMERA_DATA_FILE, std::ios_base::in);
	//for(const auto dat : camDat.imageData())
	//{
	//	unsigned int val;
	//	compareCamDat >> val;
	//	ASSERT_EQ(dat, val);
	//}
	//compareCamDat.close();
}


TEST(TestGazeboEngine, JointPlugin)
{
	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboJSONConfigConst::GazeboWorldFile, ""}});

	GazeboJSONConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_JOINT_PLUGIN_FILE;

	conf.userProcEnvParams().push_back("GAZEBO_MODEL_PATH=" TEST_GAZEBO_MODELS_DIR ":$GAZEBO_MODEL_PATH");

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<GazeboEngineJSONNRPClient>::shared_ptr engine = std::dynamic_pointer_cast<GazeboEngineJSONNRPClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_NO_THROW(engine->initialize());

	// Test device data getting
	auto devices = engine->requestOutputDevices({DeviceIdentifier("youbot::base_footprint_joint", conf.engineName(), PhysicsJoint::TypeName.data())});
	ASSERT_EQ(devices.size(), 1);

	const PhysicsJoint *pJointDev = dynamic_cast<const PhysicsJoint*>(devices[0].get());
	ASSERT_EQ(pJointDev->position(), 0);

	// Test device data setting
	const auto newTargetPos = 2.0f;

	PhysicsJoint newJointDev(DeviceIdentifier(pJointDev->id()));
	newJointDev.setEffort(NAN);
	newJointDev.setVelocity(NAN);
	newJointDev.setPosition(newTargetPos);

	ASSERT_NO_THROW(engine->handleInputDevices({&newJointDev}));
}

TEST(TestGazeboEngine, LinkPlugin)
{
	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboJSONConfigConst::GazeboWorldFile, ""}});

	GazeboJSONConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_LINK_PLUGIN_FILE;

	conf.userProcEnvParams().push_back("GAZEBO_MODEL_PATH=" TEST_GAZEBO_MODELS_DIR ":$GAZEBO_MODEL_PATH");

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<GazeboEngineJSONNRPClient>::shared_ptr engine = std::dynamic_pointer_cast<GazeboEngineJSONNRPClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_NO_THROW(engine->initialize());

	// Test device data getting
	auto devices = engine->requestOutputDevices({DeviceIdentifier("link_youbot::base_footprint", conf.engineName(), PhysicsJoint::TypeName.data())});
	ASSERT_EQ(devices.size(), 1);

	const PhysicsLink *pLinkDev = dynamic_cast<const PhysicsLink*>(devices[0].get());
	ASSERT_NE(pLinkDev, nullptr);

	// TODO: Check that link state is correct
}
