#include <gtest/gtest.h>

#include "nrp_gazebo_mpi_engine/config/gazebo_config.h"
#include "nrp_gazebo_mpi_engine/config/cmake_constants.h"
#include "nrp_gazebo_mpi_engine/devices/physics_camera.h"
#include "nrp_gazebo_mpi_engine/devices/physics_joint.h"
#include "nrp_gazebo_mpi_engine/devices/physics_link.h"
#include "nrp_gazebo_mpi_engine/nrp_client/nrp_gazebo_mpi_client.h"
#include "nrp_general_library/process_launchers/process_launcher_basic.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "tests/test_env_cmake.h"

#include <fstream>

TEST(TestGazeboEngine, Start)
{
	PythonInterpreterState pyState;

	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboConfigConst::GazeboWorldFile, ""}});

	GazeboConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_EMPTY_WORLD_FILE;
	conf.maxWorldLoadTime() = 1;

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	std::cout << confHolder.Data.dump(4, ' ') << std::endl;

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<NRPGazeboMPIClient>::shared_ptr engine = std::dynamic_pointer_cast<NRPGazeboMPIClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_ANY_THROW(engine->initialize());
}

TEST(TestGazeboEngine, WorldPlugin)
{
	PythonInterpreterState pyState;

	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboConfigConst::GazeboWorldFile, ""}});

	GazeboConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_WORLD_PLUGIN_FILE;

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<NRPGazeboMPIClient>::shared_ptr engine = std::dynamic_pointer_cast<NRPGazeboMPIClient>(
	        launcher.launchEngine(confHolder, ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic())));

	ASSERT_NE(engine, nullptr);

	ASSERT_NO_THROW(engine->initialize());

	ASSERT_NO_THROW(engine->runLoopStep(1.0f));
	ASSERT_NO_THROW(engine->waitForStepCompletion(5.0f));

	ASSERT_NO_THROW(engine->shutdown());
}

TEST(TestGazeboEngine, CameraPlugin)
{
	PythonInterpreterState pyState;

	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboConfigConst::GazeboWorldFile, ""}});

	GazeboConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_CAMERA_PLUGIN_FILE;

	conf.userProcEnvParams().push_back("GAZEBO_MODEL_PATH=" TEST_GAZEBO_MODELS_DIR ":$GAZEBO_MODEL_PATH");

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<NRPGazeboMPIClient>::shared_ptr engine = std::dynamic_pointer_cast<NRPGazeboMPIClient>(
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
	PythonInterpreterState pyState;

	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboConfigConst::GazeboWorldFile, ""}});

	GazeboConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_JOINT_PLUGIN_FILE;

	conf.userProcEnvParams().push_back("GAZEBO_MODEL_PATH=" TEST_GAZEBO_MODELS_DIR ":$GAZEBO_MODEL_PATH");

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<NRPGazeboMPIClient>::shared_ptr engine = std::dynamic_pointer_cast<NRPGazeboMPIClient>(
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

	PhysicsJoint newJointDev(pJointDev->id());
	newJointDev.setEffort(NAN);
	newJointDev.setVelocity(NAN);
	newJointDev.setPosition(newTargetPos);

	ASSERT_NO_THROW(engine->handleInputDevices({&newJointDev}));
}

TEST(TestGazeboEngine, LinkPlugin)
{
	PythonInterpreterState pyState;

	// Setup config
	ConfigStorage<nlohmann::json> confHolder;
	confHolder.Data = nlohmann::json({{GazeboConfigConst::GazeboWorldFile, ""}});

	GazeboConfig conf(confHolder);
	conf.gazeboRNGSeed() = 12345;
	conf.gazeboWorldFile() = TEST_LINK_PLUGIN_FILE;

	conf.userProcEnvParams().push_back("GAZEBO_MODEL_PATH=" TEST_GAZEBO_MODELS_DIR ":$GAZEBO_MODEL_PATH");

	conf.userProcStartParams().push_back("--verbose");

	confHolder.Data = conf.writeConfig();

	// Launch gazebo server
	GazeboEngineJSONLauncher launcher;
	PtrTemplates<NRPGazeboMPIClient>::shared_ptr engine = std::dynamic_pointer_cast<NRPGazeboMPIClient>(
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
