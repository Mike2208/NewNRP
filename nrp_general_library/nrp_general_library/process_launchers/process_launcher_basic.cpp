#include "nrp_general_library/process_launchers/process_launcher_basic.h"

#include "nrp_general_library/process_launchers/process_launcher_manager.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

ProcessLauncherBasic::~ProcessLauncherBasic()
{
	// Stop engine process if it's still running
	this->stopEngineProcess(60);
}

pid_t ProcessLauncherBasic::launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
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
			// Force quit if signal can't be created
			std::cerr << "Couldn't create parent kill signal. Error Code: " << prSig << "\nExiting...\n";
			std::cerr.flush();
			exit(prSig);
		}

		// Force quit if parent pid has changed before PR_SET_PDEATHSIG signal could be setup, preventing race condition
		if(getppid() != ppid)
		{
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
		startParamPtrs.push_back(ProcessLauncherBasic::EnvCfgCmd.data());

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
		auto res = execvp(ProcessLauncherBasic::EnvCfgCmd.data(), const_cast<char *const *>(startParamPtrs.data()));

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
		std::invalid_argument err("Forking engine child process failed");
		std::cerr << err.what();
		throw err;
	}
}

//pid_t ProcessLauncherBasic::launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
//                                                const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv)
//{
//	// Parent PID
//	const auto ppid = getpid();

//	// Fork process
//	const auto pid = fork();
//	if(pid == 0)
//	{
//		// Child process, setup environment, start Engine

//		// Setup signal that closes process if parent process quits
//		if(const auto prSig = prctl(PR_SET_PDEATHSIG, SIGHUP) < 0)
//		{
//			// Force quit if signal can't be created
//			std::cerr << "Couldn't create parent kill signal. Error Code: " << prSig << "\nExiting...\n";
//			std::cerr.flush();
//			exit(prSig);
//		}

//		// Force quit if parent pid has changed before PR_SET_PDEATHSIG signal could be setup, preventing race condition
//		if(getppid() != ppid)
//		{
//			std::cerr << "Parent process stopped unexpectedly.\nExiting...\n";
//			std::cerr.flush();
//			exit(-1);
//		}


//		// Setup environment variables in a char* vector. See definition of execvpe() for details

//		// Clear environment if it shouldn't be passed to engine
//		if(!appendParentEnv)
//			clearenv();

//		// Modify child environment variables
//		ProcessLauncherBasic::appendEnvVars(additionalEnvParams);
//		ProcessLauncherBasic::appendEnvVars(engineConfig.allEngineProcEnvParams());

//		// Setup start parameters in a char* vector. See definition of execvpe() for details
//		const auto engineProcStartParams = engineConfig.allEngineProcStartParams();

//		std::vector<const char*> startParamPtrs;
//		startParamPtrs.reserve(additionalStartParams.size() + engineProcStartParams.size() + 2);

//		startParamPtrs.push_back(engineConfig.engineProcCmd().data());

//		for(const auto &curParam : engineProcStartParams)
//			startParamPtrs.push_back(curParam.data());

//		for(const auto &curParam : additionalStartParams)
//			startParamPtrs.push_back(curParam.data());

//		// Add param end
//		startParamPtrs.push_back(nullptr);

//		// Print environment
//		const char *const *pEnvVar = environ;
//		while(*pEnvVar != nullptr)
//		{
//			std::cout << *pEnvVar << std::endl;
//			pEnvVar++;
//		}

//		// Start engine, stop current execution
//		auto res = execvp(engineConfig.engineProcCmd().data(), const_cast<char *const *>(startParamPtrs.data()));

//		std::cerr << "Couldn't start Engine with cmd \"" << engineConfig.engineProcCmd().data() << "\"\n Error code: " << res << std::endl;
//		std::cerr.flush();

//		// If the exec call fails, exit the child process without any further processing. Prevents the child process from assuming it's the main proc
//		exit(res);

//		return -1;		// Not executed, only here to prevent compiler warning
//	}
//	else if(pid > 0)
//	{
//		// Parent process, return child PID
//		this->_enginePID = pid;
//		return pid;
//	}
//	else
//	{
//		// Fork failed, throw error
//		std::invalid_argument err("Forking engine child process failed");
//		std::cerr << err.what();
//		throw err;
//	}
//}

pid_t ProcessLauncherBasic::stopEngineProcess(unsigned int killWait)
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

void ProcessLauncherBasic::appendEnvVars(const EngineConfigConst::string_vector_t &envVars)
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
