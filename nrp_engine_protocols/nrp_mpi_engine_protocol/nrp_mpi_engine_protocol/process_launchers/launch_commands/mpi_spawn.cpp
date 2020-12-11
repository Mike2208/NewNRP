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

#include "nrp_general_library/process_launchers/launch_commands/mpi_spawn.h"

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"
#include "nrp_general_library/utils/mpi_setup.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <mpi.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

MPISpawn::~MPISpawn()
{
	// Stop engine process if it's still running
	this->stopEngineProcess(10);
}

pid_t MPISpawn::launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
                                     const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv)
{
	MPISetup::initializeOnce(0, nullptr);

	// Modify child environment variables
	const auto engineProcEnvVars = engineConfig.allEngineProcEnvParams();

	// Setup start parameters in a char* vector. See definition of execvpe() for details
	const auto engineProcStartParams = engineConfig.allEngineProcStartParams();

	std::vector<const char*> startParamPtrs;

	// Reserve variable space (EnvCfgCmd + ENV_VAR1=ENV_VAL1 + ENV_VAR2=ENV_VAL2 + ... + engineProcCmd + --param1 + --param2 + ... + nullptr)
	startParamPtrs.reserve(!(appendParentEnv) + additionalEnvParams.size() + engineProcEnvVars.size() + additionalStartParams.size() + engineProcStartParams.size() + 2);

	if(!appendParentEnv)
		startParamPtrs.push_back(NRP_CLEAR_ENV);

	// Environment variables
	for(const auto &curParam : additionalEnvParams)
		startParamPtrs.push_back(curParam.data());

	for(const auto &curParam : engineProcEnvVars)
		startParamPtrs.push_back(curParam.data());

	// Engine Exec cmd
	startParamPtrs.push_back(engineConfig.engineProcCmd().data());

	// Engine start parameters
	for(const auto &curParam : engineProcStartParams)
		startParamPtrs.push_back(curParam.data());

	for(const auto &curParam : additionalStartParams)
		startParamPtrs.push_back(curParam.data());

	// Parameter end
	startParamPtrs.push_back(nullptr);

	// Spawn engine (waits for MPI_INIT() function call in child process)
	int errc;
	MPI_Comm_spawn(MPISpawn::EnvCfgCmd.data(), const_cast<char**>(startParamPtrs.data()), 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &this->_intercomm, &errc);

	if(errc != MPI_SUCCESS)
	{
		const auto errMsg = "Failed to launch engine \"" + engineConfig.engineName() + "\" with command \"" + engineConfig.engineProcCmd() + "\": "
		        + MPISetup::getErrorString(errc);
		throw NRPException::logCreate(errMsg);
	}

	// Get child pid
	try{	this->_enginePID = MPISetup::getInstance()->recvPID(this->_intercomm, 0);	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Failed to communicate with engine \"" + engineConfig.engineName() + "\" after launch");
	}

	return this->_enginePID;
}

pid_t MPISpawn::stopEngineProcess(unsigned int killWait)
{
	if(this->_enginePID > 0)
	{
		MPI_Comm_free(&this->_intercomm);

		// Send SIGTERM to gracefully stop process
		kill(this->_enginePID, SIGTERM);

		// Set to maximum wait time if time is set to 0
		if(killWait == 0)
			killWait = std::numeric_limits<unsigned int>::max();

		// Check Nest status. After _killWait seconds, send SIGKILL to force a shutdown
		const auto end = std::chrono::system_clock::now() + std::chrono::duration<size_t>(killWait);

		bool pKilled = false;
		do
		{
			int status = 0;
			if(waitpid(this->_enginePID, &status, WNOHANG | WUNTRACED) == this->_enginePID)
			{
				pKilled = true;
				break;
			}

			// Sleep for 10ms between checks
			usleep(10*1000);
		}
		while(std::chrono::system_clock::now() < end);

		// Force shutdown
		if(!pKilled)
			kill(this->_enginePID, SIGKILL);

		this->_enginePID = -1;
		this->_intercomm = MPI_COMM_NULL;
	}

	return 0;
}

MPI_Comm MPISpawn::getIntercomm() const
{	return this->_intercomm;	}

pid_t MPISpawn::getEnginePID() const
{	return this->_enginePID;	}

void MPISpawn::appendEnvVars(const EngineConfigConst::string_vector_t &envVars)
{
	// Modify child environment variables
	for(auto &envVar : envVars)
	{
//		const auto splitVar = ProcessLauncherBasic::splitEnvVar(envVar);
//		if(std::get<0>(splitVar).empty() ||
//		        setenv(std::get<0>(splitVar).data(), std::get<1>(splitVar).data(), 1))
//		{
//			const auto errMsg = std::string("Failed to add environment variable:\n") + envVar.data();
//			std::cerr << errMsg << std::endl;
//			throw std::logic_error(errMsg);
//		}

		const std::string envCmd = "export " + envVar;
		if(system(envCmd.data()) != 0)
			throw NRPException::logCreate(std::string("Failed to add environment variable:\n") + envVar.data());
	}
}
