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

#ifndef NRP_COMMUNICATION_CONTROLLER_H
#define NRP_COMMUNICATION_CONTROLLER_H

#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_server.h"
#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"

#include "nrp_gazebo_grpc_engine/config/gazebo_grpc_config.h"
#include "nrp_gazebo_grpc_engine/engine_server/gazebo_step_controller.h"

#include <pistache/router.h>
#include <pistache/endpoint.h>

#include <gazebo/common/Plugin.hh>
#include <map>
#include <memory>

/*!
 * \brief Manages communication with the NRP. Uses a REST server to send/receive data. Singleton class.
 */
class NRPCommunicationController
        : public EngineGrpcServer
{
	public:
		~NRPCommunicationController() override;

		/*! \brief Delete for singleton */
		NRPCommunicationController(const NRPCommunicationController &other) = delete;

		/*! \brief Delete for singleton */
		NRPCommunicationController &operator=(const NRPCommunicationController &other) = delete;

		/*! \brief Delete for singleton */
		NRPCommunicationController(NRPCommunicationController &&other) = delete;

		/*! \brief Delete for singleton */
		NRPCommunicationController &&operator=(NRPCommunicationController &&other) = delete;

		/*!
		 * \brief Get singleton instance
		 * \return Gets instance of NRPCommunicationController
		 */
		static NRPCommunicationController& getInstance();

		/*!
		 * \brief Reset server with the given server URL
		 * \param serverURL URL used by server
		 * \return Returns reference to server instance
		 */
		static NRPCommunicationController& resetInstance(const std::string &serverURL);

		/*!
		 * \brief Reset server with the given server URL
		 * \param serverURL URL used by server
		 * \param engineName Name of this engine
		 * \param registrationURL URL used to register this engine server's URL
		 * \return Returns reference to server instance
		 */
		static NRPCommunicationController& resetInstance(const std::string &serverURL, const std::string &engineName, const std::string &registrationURL);


		/*!
		 * \brief Register a step controller
		 * \param stepController Pointer to step controller
		 */
		void registerStepController(GazeboStepController *stepController);

		/*!
		 * \brief Create device name from the given plugin and sensor/joint/link
		 * \tparam T Plugin Type
		 * \param plugin Controller Plugin
		 * \param objectName Name of the controlled object (sensor, joint, link, ...)
		 * \return Returns device name
		 */
		template<class T>
		static std::string createDeviceName(const gazebo::PluginT<T> &plugin, const std::string &objectName)
		{	return plugin.GetHandle() + "::" + objectName;	}

	private:

		/*!
		 * \brief Singleton instance of this class
		 */
		static std::unique_ptr<NRPCommunicationController> _instance;

		/*!
		 * \brief Controlls gazebo stepping
		 */
		GazeboStepController *_stepController = nullptr;

		virtual SimulationTime runLoopStep(SimulationTime timeStep) override;

		virtual void initialize(const nlohmann::json &data, EngineGrpcServer::lock_t &deviceLock) override;

		virtual void shutdown(const nlohmann::json &data) override;

		/*!
		 * \brief Make private for singleton
		 */
		NRPCommunicationController() = default;

		/*!
		 * \brief Constructor. Private for singleton
		 * \param address Server Address
		 */
		NRPCommunicationController(const std::string &address);

		/*!
		 * \brief Constructor. Private for singleton
		 * \param serverURL URL used by server
		 * \param engineName Name of this engine
		 * \param registrationURL URL used to register this engine server's URL
		 * \return Returns reference to server instance
		 */
		NRPCommunicationController(const std::string &serverURL, const std::string &engineName, const std::string &registrationURL);
};

#endif
