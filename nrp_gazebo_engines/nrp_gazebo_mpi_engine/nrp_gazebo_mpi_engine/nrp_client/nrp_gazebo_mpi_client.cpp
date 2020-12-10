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

#include "nrp_gazebo_mpi_engine/nrp_client/nrp_gazebo_mpi_client.h"

#include "nrp_general_library/process_launchers/launch_commands/mpi_spawn.h"

#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>

NRPGazeboMPIClient::NRPGazeboMPIClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : NRPMPIClient(config, std::move(launcher))
{}

//void NRPGazeboMPIClient::initialize()
//{
//	this->_comm = this->getComm();

//	auto confDat = this->engineConfig()->writeConfig();
//	MPICommunication::sendString(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, confDat.dump());

//}

//void NRPGazeboMPIClient::shutdown()
//{
//}
