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

		virtual SimulationTime getEngineTime() const override
		{	return SimulationTime::zero();	}

		virtual void runLoopStep(SimulationTime) override
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
