#include <gtest/gtest.h>

#include "nrp_general_library/process_launchers/process_launcher_basic.h"
#include "nrp_general_library/utils/pipe_communication.h"

#include "tests/test_env_cmake.h"

struct TestEngineConfig
        : public EngineConfig<TestEngineConfig, PropNames<> >
{
	static constexpr FixedString ConfigType = "TestEngineConfig";

	TestEngineConfig()
	    : EngineConfig(ConfigType, nlohmann::json())
	{}
};

TEST(ProcessLauncherBasicTest, TestLaunch)
{
	ProcessLauncherBasic launcher;
	PipeCommunication pCommPtC;
	PipeCommunication pCommCtP;

	// Add test params and envs
	std::vector<std::string> additionalParams;
	additionalParams.push_back(std::to_string(pCommPtC.readFd()));
	additionalParams.push_back(std::to_string(pCommCtP.writeFd()));

	std::vector<std::string> additionalEnvVars;
	additionalEnvVars.push_back(TEST_PROC_ENV_VAR_NAME "=" TEST_PROC_ENV_VAR_VAL);

	TestEngineConfig testCfg;
	const std::string procCmd(TEST_NRP_PROCESS_EXEC);
	testCfg.EngineConfigGeneral::engineProcCmd() = procCmd;

	// Fork engine process
	ASSERT_GE(launcher.launchEngineProcess(testCfg, additionalEnvVars, additionalParams), 0);

	pCommCtP.closeWrite();
	pCommPtC.closeRead();

	// Sync processes
	char readDat[50] = "";
	pCommCtP.readP(readDat, sizeof(TEST_PROC_STR_START), 5, 1);
	ASSERT_STREQ(readDat, TEST_PROC_STR_START);

	pCommPtC.writeP(TEST_PROC_STR_START, sizeof(TEST_PROC_STR_START), 5, 1);

	// Test that env value was set properly
	ASSERT_EQ(pCommCtP.readP(readDat, sizeof(TEST_PROC_ENV_VAR_VAL), 5, 1), sizeof(TEST_PROC_ENV_VAR_VAL));
	ASSERT_STREQ(readDat, TEST_PROC_ENV_VAR_VAL);

	// Stop engine
	ASSERT_LE(launcher.stopEngineProcess(5), 0);

	ASSERT_EQ(pCommCtP.readP(readDat, sizeof(TEST_PROC_STR_SIGTERM), 5, 1), sizeof(TEST_PROC_STR_SIGTERM));
	ASSERT_STREQ(readDat, TEST_PROC_STR_SIGTERM);
}
