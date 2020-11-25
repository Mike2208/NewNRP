#ifndef NEST_ENGINE_SERVER_NRP_CLIENT_H
#define NEST_ENGINE_SERVER_NRP_CLIENT_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/plugin_system/plugin.h"

#include "nrp_nest_server_engine/devices/nest_server_device.h"

#include "nrp_nest_server_engine/config/nest_server_config.h"

#include <future>
#include <unistd.h>

/*!
 * \brief NRP - Nest Communicator on the NRP side. Converts DeviceInterface classes from/to JSON objects
 */
class NestEngineServerNRPClient
        : public Engine<NestEngineServerNRPClient, NestServerConfig>
{
		struct CompNestDevs
		{
			bool operator()(const NestServerDevice::shared_ptr &lhs, const NestServerDevice::shared_ptr &rhs) const
			{	return lhs->id().Name.compare(rhs->id().Name) < 0;	}
		};

		struct nest_devices_t : public std::set<NestServerDevice::shared_ptr, CompNestDevs>
		{
			iterator find(const std::string &name)
			{
				for(auto nestDevIt = this->begin(); nestDevIt != this->end(); ++nestDevIt)
				{
					if(nestDevIt->get()->id().Name == name)
						return nestDevIt;
				}

				return this->end();
			}
		};

		/*!
		 * \brief Number of seconds to wait for Nest to exit cleanly after first SIGTERM signal. Afterwards, send a SIGKILL
		 */
		static constexpr size_t _killWait = 10;

	public:
		NestEngineServerNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~NestEngineServerNRPClient() override;

		virtual RESULT initialize() override;
		virtual RESULT shutdown() override;

		virtual float getEngineTime() const override;

		virtual step_result_t runLoopStep(float timeStep) override;
		virtual RESULT waitForStepCompletion(float timeOut) override;

		virtual RESULT handleInputDevices(const device_inputs_t &inputDevices) override;

	protected:
		virtual device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t &deviceIdentifiers) override;

	private:
		std::future<EngineInterface::RESULT> _runStepThread;
		nest_devices_t _nestDevs;

		EngineInterface::RESULT runStepFcn(float timestep);
		std::string serverAddress() const;
};

using NestEngineServerNRPClientLauncher = NestEngineServerNRPClient::EngineLauncher<NestServerConfig::DefEngineType>;


CREATE_NRP_ENGINE_LAUNCHER(NestEngineServerNRPClientLauncher);


#endif // NEST_ENGINE_SERVER_NRP_CLIENT_H
