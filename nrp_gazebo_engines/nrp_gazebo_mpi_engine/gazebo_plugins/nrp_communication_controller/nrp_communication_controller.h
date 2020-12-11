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

#include "nrp_gazebo_mpi_engine/config/gazebo_config.h"
#include "nrp_gazebo_mpi_engine/engine_server/gazebo_step_controller.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/engine_server/engine_mpi_server.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"

#include <pistache/router.h>
#include <pistache/endpoint.h>

#include <gazebo/common/Plugin.hh>
#include <map>
#include <memory>

/*!
 * \brief Manages communication with the NRP. Uses a REST server to send/receive data. Singleton class.
 */
class NRPCommunicationController
        : public EngineMPIServer
{
	public:
		~NRPCommunicationController();

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
		 * \param nrpComm MPI Communicator to interact with server
		 * \return Returns reference to server instance
		 */
		static NRPCommunicationController& resetInstance(MPI_Comm nrpComm = EngineMPIServer::getNRPComm());

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

		void initialize(const std::string &initData) override;
		void shutdown(const std::string &shutdownData) override;

		EngineInterface::step_result_t runLoopStep(SimulationTime timeStep) override;
		SimulationTime getSimTime() const override;

		/*!
		 * \brief Constructor. Private for singleton
		 * \param parentComm MPI Communicator to NRP
		 */
		explicit NRPCommunicationController(MPI_Comm nrpComm);
};

#endif
