#ifndef MPI_SPAWN_H
#define MPI_SPAWN_H

#include "nrp_general_library/config_headers/nrp_cmake_constants.h"
#include "nrp_general_library/process_launchers/launch_commands/launch_command.h"

#include <mpi.h>

class MPISpawn
        : public LaunchCommand<"MPI">
{
		/*!
		 *	\brief Command to set environment variables. More versatile than the C function setenv
		 */
		static constexpr std::string_view EnvCfgCmd = NRP_ENGINE_SET_ENV_CMD;

	public:
		~MPISpawn() override;

		pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
		                          const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv = true) override;

		pid_t stopEngineProcess(unsigned int killWait) override;

		/*!
		 * \brief Get MPI Communicator with child
		 */
		MPI_Comm getIntercomm() const;

	private:
		/*!
		 * \brief Communicator with child
		 */
		MPI_Comm _intercomm = MPI_COMM_NULL;

		/*!
		 * \brief Child PID
		 */
		pid_t _enginePID = -1;

		/*!
		 * \brief Add environment variables to environ
		 * \param envVars Environment variables
		 */
		static void appendEnvVars(const EngineConfigConst::string_vector_t &envVars);
};

#endif // MPI_SPAWN_H
