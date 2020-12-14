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

#include "nrp_general_library/process_launchers/launch_commands/basic_fork.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

BasicFork::~BasicFork()
{
	// Stop engine process if it's still running
	this->stopEngineProcess(60);
}

pid_t BasicFork::launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
                                     const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv)
{
	// Parent PID
	const auto ppid = getpid();

	// Fork process
	const auto pid = fork();
	if(pid == 0)
	{
		// Child process, setup environment, start Engine

		// Setup signal that closes process if parent process quits
		if(const auto prSig = prctl(PR_SET_PDEATHSIG, SIGHUP) < 0)
		{
			// Force quit if signal can't be created (Don't use the logger here, as this is a separate process)
			std::cerr << "Couldn't create parent kill signal. Error Code: " << prSig << "\nExiting...\n";
			std::cerr.flush();
			exit(prSig);
		}

		// Force quit if parent pid has changed before PR_SET_PDEATHSIG signal could be setup, preventing race condition
		if(getppid() != ppid)
		{
			// Don't use the logger here, as this is a separate process
			std::cerr << "Parent process stopped unexpectedly.\nExiting...\n";
			std::cerr.flush();
			exit(-1);
		}


		// Setup environment variables in a char* vector. See definition of execvpe() for details

		// Clear environment if it shouldn't be passed to engine
		if(!appendParentEnv)
			clearenv();

		// Modify child environment variables
		const auto engineProcEnvVars = engineConfig.allEngineProcEnvParams();
		//ProcessLauncherBasic::appendEnvVars(additionalEnvParams);

		// Setup start parameters in a char* vector. See definition of execvpe() for details
		const auto engineProcStartParams = engineConfig.allEngineProcStartParams();

		std::vector<const char*> startParamPtrs;

		// Reserve variable space (EnvCfgCmd + ENV_VAR1=ENV_VAL1 + ENV_VAR2=ENV_VAL2 + ... + engineProcCmd + --param1 + --param2 + ... + nullptr)
		startParamPtrs.reserve(additionalEnvParams.size() + engineProcEnvVars.size() + additionalStartParams.size() + engineProcStartParams.size() + 3);

		// Environment set command
		startParamPtrs.push_back(BasicFork::EnvCfgCmd.data());

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

		std::cout << "Current engine PID: " <<getpid() << std::endl;

		// Start engine, stop current execution
		auto res = execvp(BasicFork::EnvCfgCmd.data(), const_cast<char *const *>(startParamPtrs.data()));

		// Don't use the logger here, as this is a separate process
		std::cerr << "Couldn't start Engine with cmd \"" << engineConfig.engineProcCmd().data() << "\"\n Error code: " << res << std::endl;
		std::cerr.flush();

		// If the exec call fails, exit the child process without any further processing. Prevents the child process from assuming it's the main proc
		exit(res);

		return -1;		// Not executed, only here to prevent compiler warning
	}
	else if(pid > 0)
	{
		// Parent process, return child PID
		this->_enginePID = pid;
		return pid;
	}
	else
	{
		// Fork failed, throw error
		throw NRPException::logCreate("Forking engine child process failed");
	}
}

pid_t BasicFork::stopEngineProcess(unsigned int killWait)
{
	if(this->_enginePID > 0)
	{
		// Send SIGTERM to gracefully stop Nest process
		kill(this->_enginePID, SIGTERM);

		// Set to maximum wait time if time is set to 0
		if(killWait == 0)
			killWait = std::numeric_limits<unsigned int>::max();

		// Check Nest status. After _killWait seconds, send SIGKILL to force a shutdown
		const auto end = std::chrono::system_clock::now() + std::chrono::duration<size_t>(killWait);

		bool pKilled = false;
		do
		{
			// Check if engine process has stopped
			if(this->getProcessStatus() == ENGINE_RUNNING_STATUS::STOPPED)
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
	}

	return 0;
}

LaunchCommandInterface::ENGINE_RUNNING_STATUS BasicFork::getProcessStatus()
{
	// Check if engine was already stopped before
	if(this->_enginePID < 0)
		return ENGINE_RUNNING_STATUS::RUNNING;

	// Check if this process received a stop notification for this child PID
	int engineStatus;
	if(waitpid(this->_enginePID, &engineStatus, WNOHANG | WUNTRACED) == this->_enginePID)
	{
		this->_enginePID = -1;
		return ENGINE_RUNNING_STATUS::STOPPED;
	}
	else
		return ENGINE_RUNNING_STATUS::RUNNING;
}

void BasicFork::appendEnvVars(const EngineConfigConst::string_vector_t &envVars)
{
	// Modify child environment variables
	for(auto &envVar : envVars)
	{
		const std::string envCmd = "export " + envVar;
		if(system(envCmd.data()) != 0)
			throw NRPExceptionNonRecoverable(std::string("Failed to add environment variable:\n") + envVar.data());
	}
}
