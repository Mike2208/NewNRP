#include <gtest/gtest.h>

#include "nrp_general_library/plugin_system/plugin_manager.h"
#include "tests/test_env_cmake.h"

using namespace testing;

TEST(PluginManagerTest, TestLoadPlugin)
{
	PluginManager pluginMan;

	ASSERT_NO_THROW(pluginMan.addPluginPath(TEST_PLUGIN_DIR));

	// Test loading plugin
	auto engineLauncher = pluginMan.loadPlugin(TEST_NRP_PLUGIN);
	ASSERT_NE(engineLauncher, nullptr);
}
