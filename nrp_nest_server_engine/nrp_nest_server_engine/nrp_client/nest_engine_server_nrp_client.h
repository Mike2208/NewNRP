/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

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

		virtual void initialize() override;
		virtual void shutdown() override;

		virtual SimulationTime getEngineTime() const override;

		virtual void runLoopStep(SimulationTime timeStep) override;
		virtual void waitForStepCompletion(float timeOut) override;

		virtual void handleInputDevices(const device_inputs_t &inputDevices) override;

	protected:
		virtual device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t &deviceIdentifiers) override;

	private:
		std::future<bool> _runStepThread;
		nest_devices_t _nestDevs;

		bool runStepFcn(SimulationTime timestep);
		std::string serverAddress() const;
};

using NestEngineServerNRPClientLauncher = NestEngineServerNRPClient::EngineLauncher<NestServerConfig::DefEngineType>;


CREATE_NRP_ENGINE_LAUNCHER(NestEngineServerNRPClientLauncher);


#endif // NEST_ENGINE_SERVER_NRP_CLIENT_H
