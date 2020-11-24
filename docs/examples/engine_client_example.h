#include <future>
#include <nrp_general_library/engine_interfaces/engine_interface.h>

class EngineClientExample
    : public Engine<EngineClientExample, EngineConfigBase>
{
	public:
		static constexpr FixedString DefEngineName = "example_engine";

		EngineClientExample(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
		    : Engine<EngineClientExample, EngineConfigBase>(config, std::move(launcher))
		{}

		virtual typename EngineInterface::step_result_t runLoopStep(float timeStep) override
		{
			this->_loopStepThread = std::async(std::launch::async, std::bind(&EngineClientExample::sendRunLoopStepCommand, this, timeStep));
			return EngineInterface::SUCCESS;
		}

		float sendRunLoopStepCommand(float timeStep);

		virtual typename EngineInterface::RESULT waitForStepCompletion(float timeOut) override
		{
			// If thread state is invalid, loop thread has completed and waitForStepCompletion was called once before
			if(!this->_loopStepThread.valid())
			{
				return EngineInterface::SUCCESS;
			}

			// Wait until timeOut has passed
			if(timeOut > 0)
			{
				if(this->_loopStepThread.wait_for(std::chrono::duration<double>(timeOut)) != std::future_status::ready)
					return EngineInterface::ERROR;
			}
			else
				this->_loopStepThread.wait();

			this->_engineTime = this->_loopStepThread.get();
			return EngineInterface::SUCCESS;
		}

		float getEngineTime() const override
		{
			return this->_engineTime;
		}

		virtual typename EngineInterface::RESULT handleInputDevices(const typename EngineInterface::device_inputs_t &inputDevices) override;

		virtual typename EngineInterface::device_outputs_set_t requestOutputDeviceCallback(const typename EngineInterface::device_identifiers_t &deviceIdentifiers) override;

	private:
		float _engineTime     = 0.0f;
		std::future<float> _loopStepThread;
};
