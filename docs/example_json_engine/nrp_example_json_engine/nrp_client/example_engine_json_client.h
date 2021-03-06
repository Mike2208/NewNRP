#ifndef EXAMPLE_ENGINE_CLIENT_H
#define EXAMPLE_ENGINE_CLIENT_H

#include "nrp_example_json_engine/config/example_json_config.h"
#include "nrp_example_json_engine/devices/example_device.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"

#include <future>

class ExampleEngineClient
        : public Engine<ExampleEngineClient, ExampleJSONConfig>
{
	public:
		ExampleEngineClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
		    : Engine(config, std::move(launcher))
		{}

		virtual ~ExampleEngineClient() override;

		virtual void initialize() override;
		virtual void shutdown() override;

		virtual float getEngineTime() const override;
		virtual step_result_t runLoopStep(float timeStep) override
		{
			this->_loopStepThread = std::async(std::launch::async, std::bind(&ExampleEngineClient::sendRunLoopStepCommand, this, timeStep));
			return EngineInterface::SUCCESS;
		}

		float sendRunLoopStepCommand(float timeStep);

		virtual void waitForStepCompletion(float timeOut) override
		{
			// If thread state is invalid, loop thread has completed and waitForStepCompletion was called once before
			if(!this->_loopStepThread.valid())
				return;

			// Wait until timeOut has passed
			if(timeOut > 0)
			{
				if(this->_loopStepThread.wait_for(std::chrono::duration<double>(timeOut)) != std::future_status::ready)
					throw NRPException::logCreate("Engine \"" + this->engineName() + "\" loop is taking too long to complete");
			}
			else
				this->_loopStepThread.wait();

			this->_engineTime = this->_loopStepThread.get();
		}

		virtual void handleInputDevices(const device_inputs_t &inputDevices) override;
		virtual device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t &deviceIdentifiers) override;

	private:
		float _engineTime     = 0.0f;
		std::future<float> _loopStepThread;
};

using ExampleEngineLauncher = ExampleEngineClient::EngineLauncher<ExampleJSONConfig::DefEngineType>;

CREATE_NRP_ENGINE_LAUNCHER(ExampleEngineLauncher);


#endif // EXAMPLE_ENGINE_CLIENT_H
