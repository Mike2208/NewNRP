#include "nrp_general_library/process_launchers/launch_commands/mpi_spawn.h"

#include "nrp_general_library/utils/mpi_setup.h"

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
	this->stopEngineProcess(60);
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

	if(appendParentEnv)
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
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	// Get child pid
	errc = MPI_Recv(&this->_enginePID, sizeof(this->_enginePID), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, this->_intercomm, MPI_STATUS_IGNORE);
	if(errc != MPI_SUCCESS)
	{
		const auto errMsg = "Failed to communicate with engine \"" + engineConfig.engineName() + "\" after launch: " + MPISetup::getErrorString(errc);
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	return this->_enginePID;
}

pid_t MPISpawn::stopEngineProcess(unsigned int killWait)
{
	if(this->_enginePID > 0)
	{
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
			int engineStatus;
			if(waitpid(this->_enginePID, &engineStatus, WNOHANG) == this->_enginePID)
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

MPI_Comm MPISpawn::getIntercomm() const
{	return this->_intercomm;	}

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
		{
			const auto errMsg = std::string("Failed to add environment variable:\n") + envVar.data();
			std::cerr << errMsg << std::endl;
			throw std::logic_error(errMsg);
		}
	}
}
