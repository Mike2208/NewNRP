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

#include "nrp_communication_controller/nrp_communication_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"
#include "nrp_general_library/utils/mpi_setup.h"

void gazebo::NRPCommunicationPlugin::Load(int argc, char **argv)
{
	std::cout << "NRP Communication plugin: Initializing...\n";

	this->_pyState = PythonInterpreterState();
	MPISetup::initializeOnce(0, nullptr, true);

	// Reset communication controller
	NRPCommunicationController::resetInstance();

	std::cout << "NRP Communication plugin: MPI Initialized started. Waiting for input...\n";
	this->_commThread = std::async(std::launch::async, &NRPCommunicationPlugin::commThreadFcn);
}

void gazebo::NRPCommunicationPlugin::Reset()
{
	// Reset server
	std::cout << "NRP Communication plugin: Resetting controller...\n";
	NRPCommunicationController::resetInstance();
	std::cout << "NRP Communication plugin: Waiting for input...\n";
}

std::string gazebo::NRPCommunicationPlugin::commThreadFcn() noexcept
{
	auto &mpiComm = NRPCommunicationController::getInstance();
	EngineMPIControl controlCmd(EngineMPIControlConst::NONE, 0l);
	do
	{
		try
		{
			mpiComm.getClientCmd(controlCmd);
			mpiComm.handleClientCmd(controlCmd);
		}
		catch(std::exception &e)
		{
			return NRPException::logCreate(e, "MPI Communication Failure").what();
		}
	}
	while(mpiComm.getEngineState() != EngineMPIServer::STOPPED);

	return "";
}
