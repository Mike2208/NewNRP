#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_launcher_manager.h"
#include "nrp_general_library/process_launchers/process_launcher_basic.h"

using namespace testing;

struct TestEngine1
        : public EngineInterface
{
	TestEngine1()
	    : EngineInterface(ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()))
	{
		this->engineName() = "engine1";
	}

	virtual ~TestEngine1() override = default;

	std::shared_ptr<EngineConfigGeneral> engineConfigGeneral() const override
	{	return nullptr;		}

	RESULT initialize() override
	{	return SUCCESS;	}

	RESULT shutdown() override
	{	return SUCCESS;	}

	float getEngineTime() const override
	{	return 0;	}

	float getEngineTimestep() const override
	{	return 0;	}

	step_result_t runLoopStep(float) override
	{	return SUCCESS;	}

	RESULT waitForStepCompletion(float) override
	{	return SUCCESS;	}

	RESULT handleInputDevices(const device_inputs_t&) override
	{	return SUCCESS;	}

	protected:
	    device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t &deviceIdentifiers) override
		{
			device_outputs_set_t retVal;
			for(const auto &devID : deviceIdentifiers)
			{
				retVal.emplace(new DeviceInterface(devID));
			}

			return retVal;
		}
};

struct TestLauncher1
        : public EngineLauncherInterface
{
	TestLauncher1()
	    : EngineLauncherInterface("engine1_type")
	{}
	~TestLauncher1() override = default;

	EngineInterface::shared_ptr launchEngine(EngineConfigConst::config_storage_t&, ProcessLauncherInterface::unique_ptr&&) override
	{
		return EngineInterface::shared_ptr(new TestEngine1());
	}
};

struct TestEngine2
        : public EngineInterface
{
	TestEngine2()
	    : EngineInterface(ProcessLauncherInterface::unique_ptr(new ProcessLauncherBasic()))
	{
		this->engineName() = "engine2";
	}

	virtual ~TestEngine2() override = default;

	std::shared_ptr<EngineConfigGeneral> engineConfigGeneral() const override
	{	return nullptr;		}

	RESULT initialize() override
	{	return SUCCESS;	}

	RESULT shutdown() override
	{	return SUCCESS;	}

	float getEngineTime() const override
	{	return 0;	}

	float getEngineTimestep() const override
	{	return 0;	}

	step_result_t runLoopStep(float) override
	{	return SUCCESS;	}

	RESULT waitForStepCompletion(float) override
	{	return SUCCESS;	}

	RESULT handleInputDevices(const device_inputs_t&) override
	{	return SUCCESS;	}

	protected:
	    device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t&) override
		{	return device_outputs_set_t();	}
};

struct TestLauncher2
        : public EngineLauncherInterface
{
	TestLauncher2()
	    : EngineLauncherInterface("engine2_type")
	{}

	virtual ~TestLauncher2() override = default;

	EngineInterface::shared_ptr launchEngine(EngineConfigConst::config_storage_t&, ProcessLauncherInterface::unique_ptr&&) override
	{
		return EngineInterface::shared_ptr(new TestEngine2());
	}
};

TEST(EngineLauncherManagerTest, RegisterDevice)
{
	auto test1Launcher = EngineLauncherInterfaceSharedPtr(new TestLauncher1());
	EngineLauncherManager engines;

	ASSERT_EQ(engines.findLauncher(test1Launcher->engineType()), nullptr);

	engines.registerLauncher(test1Launcher);
	ASSERT_EQ(engines.findLauncher(test1Launcher->engineType()).get(), test1Launcher.get());

	auto test2Launcher = EngineLauncherInterfaceSharedPtr(new TestLauncher2());
	engines.registerLauncher(test2Launcher);
	ASSERT_EQ(engines.findLauncher(test2Launcher->engineType()).get(), test2Launcher.get());
}
