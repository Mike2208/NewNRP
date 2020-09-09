#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"

#include "nrp_general_library/plugin_system/plugin.h"

struct TestEngineConfig
        : public EngineJSONConfig<TestEngineConfig, PropNames<> >
{
	static constexpr FixedString ConfigType = "TestConf";

	TestEngineConfig(EngineConfigConst::config_storage_t &config)
	    : EngineJSONConfig(config)
	{}
};

class TestEngine
        : public Engine<TestEngine, TestEngineConfig>
{
	public:
		TestEngine(EngineConfigConst::config_storage_t &configHolder, ProcessLauncherInterface::unique_ptr &&launcher)
		    : Engine(configHolder, std::move(launcher))
		{}

		virtual RESULT initialize() override
		{	return SUCCESS;	}

		virtual RESULT shutdown() override
		{	return SUCCESS;	}

		virtual float getEngineTime() const override
		{	return 0.0f;	}

		virtual step_result_t runLoopStep(float) override
		{	return SUCCESS;	}

		virtual RESULT waitForStepCompletion(float) override
		{	return SUCCESS;	}

		virtual device_outputs_t getOutputDevices(const device_identifiers_t &deviceIdentifiers) override
		{	return device_outputs_t(deviceIdentifiers.size());	}

		virtual RESULT handleInputDevices(const device_inputs_t &) override
		{	return SUCCESS;	}

};

CREATE_NRP_ENGINE_LAUNCHER(TestEngine::EngineLauncher<"TestEngine">);
