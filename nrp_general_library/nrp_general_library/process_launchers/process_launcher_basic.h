#ifndef PROCESS_LAUNCHER_BASIC_H
#define PROCESS_LAUNCHER_BASIC_H

#include "nrp_general_library/config_headers/nrp_cmake_constants.h"
#include "nrp_general_library/process_launchers/process_launcher.h"

/*!
 * \brief Basic Process Launcher. Forks a new process for the engine
 */
class ProcessLauncherBasic
        : public ProcessLauncher<ProcessLauncherBasic, FixedString("Basic")>
{
		/*!
		 *	\brief Command to set environment variables. More versatile than the C function setenv
		 */
		static constexpr std::string_view EnvCfgCmd = NRP_ENGINE_SET_ENV_CMD;

	public:
		~ProcessLauncherBasic() override;

		pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
		                          const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv = true) override;

		pid_t stopEngineProcess(unsigned int killWait) override;

	private:
		/*!
		 * \brief PID of child process running the engine
		 */
		pid_t _enginePID;

		/*!
		 * \brief Add environment variables to environ
		 * \param envVars Environment variables
		 */
		static void appendEnvVars(const EngineConfigConst::string_vector_t &envVars);
};

#endif // PROCESS_LAUNCHER_BASIC_H
