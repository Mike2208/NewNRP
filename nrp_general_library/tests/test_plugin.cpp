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

		virtual SimulationTime getEngineTime() const override
		{	return SimulationTime::zero();	}

		virtual step_result_t runLoopStep(SimulationTime) override
		{	return SUCCESS;	}

		virtual RESULT waitForStepCompletion(float) override
		{	return SUCCESS;	}

		virtual RESULT handleInputDevices(const device_inputs_t &) override
		{	return SUCCESS;	}

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
