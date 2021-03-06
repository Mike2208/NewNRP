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

#ifndef NRP_GAZEBO_MPI_CLIENT_H
#define NRP_GAZEBO_MPI_CLIENT_H

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/nrp_client/nrp_mpi_client.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/plugin_system/plugin.h"

#include "nrp_gazebo_mpi_engine/devices/physics_camera.h"
#include "nrp_gazebo_mpi_engine/devices/physics_joint.h"
#include "nrp_gazebo_mpi_engine/devices/physics_link.h"

#include "nrp_gazebo_mpi_engine/config/gazebo_config.h"

#include <unistd.h>

/*! \defgroup gazebo_mpi_engine "Gazebo MPI Engine"
Gazebo Engine based on the \ref mpi_engine.
 */

/*!
 *  \brief NRP - Gazebo Communicator on the NRP side. Converts DeviceInterface classes from/to JSON objects
 */
class NRPGazeboMPIClient
        : public NRPMPIClient<NRPGazeboMPIClient, GazeboConfig, PhysicsCamera, PhysicsJoint, PhysicsLink>
{
	public:
		NRPGazeboMPIClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~NRPGazeboMPIClient() override = default;

		//virtual void initialize() override;

		//virtual void shutdown() override;
};

using GazeboEngineJSONLauncher = NRPGazeboMPIClient::EngineLauncher<GazeboConfig::DefEngineType>;

CREATE_NRP_ENGINE_LAUNCHER(GazeboEngineJSONLauncher);

#endif // NRP_GAZEBO_MPI_CLIENT_H
