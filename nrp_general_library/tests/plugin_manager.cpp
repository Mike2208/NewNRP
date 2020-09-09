#include <gtest/gtest.h>

#include "nrp_general_library/plugin_system/plugin_manager.h"
#include "tests/test_env_cmake.h"

using namespace testing;

TEST(PluginManagerTest, TestLoadPlugin)
{
	PluginManager pluginMan;

	// Test adding lib path
	ASSERT_NO_THROW(pluginMan.addPluginPath(TEST_PLUGIN_DIR));

	const char *const pEnvLibPath = getenv("LD_LIBRARY_PATH");
	ASSERT_NE(pEnvLibPath, nullptr);
	ASSERT_TRUE(std::string(pEnvLibPath).starts_with(TEST_PLUGIN_DIR));

	// Test loading plugin
	auto engineLauncher = pluginMan.loadPlugin(TEST_NRP_PLUGIN);
	ASSERT_NE(engineLauncher, nullptr);
}
