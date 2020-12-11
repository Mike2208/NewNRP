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

#ifndef MPI_SPAWN_H
#define MPI_SPAWN_H

#include "nrp_general_library/config/cmake_constants.h"
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
		MPISpawn() = default;
		~MPISpawn() override;

		pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
		                          const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv = true) override;

		pid_t stopEngineProcess(unsigned int killWait) override;

		/*!
		 * \brief Get MPI Communicator with child
		 */
		MPI_Comm getIntercomm() const;

		/*!
		 * \brief Get Engine PID
		 */
		pid_t getEnginePID() const;

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
