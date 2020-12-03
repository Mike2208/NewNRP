#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/config/engine_config.h"

#include "nrp_general_library/plugin_system/plugin.h"

struct TestEngineConfig
        : public EngineConfig<TestEngineConfig, PropNames<> >
{
	static constexpr FixedString ConfigType = "TestConf";

	TestEngineConfig(EngineConfigConst::config_storage_t &config)
	    : EngineConfig(config)
	{}
};

class TestEngine
        : public Engine<TestEngine, TestEngineConfig>
{
	public:
		TestEngine(EngineConfigConst::config_storage_t &configHolder, ProcessLauncherInterface::unique_ptr &&launcher)
		    : Engine(configHolder, std::move(launcher))
		{}

		virtual void initialize() override
		{}

		virtual void shutdown() override
		{}

		virtual float getEngineTime() const override
		{	return 0.0f;	}

		virtual void runLoopStep(float) override
		{}

		virtual void waitForStepCompletion(float) override
		{}

		virtual void handleInputDevices(const device_inputs_t &) override
		{}

	protected:
		virtual device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t &deviceIdentifiers) override
		{
			device_outputs_set_t retVal;
			for(const auto &devID : deviceIdentifiers)
			{
				retVal.emplace(new DeviceInterface(devID));
			}

			return retVal;
		}
};

CREATE_NRP_ENGINE_LAUNCHER(TestEngine::EngineLauncher<"TestEngine">);
