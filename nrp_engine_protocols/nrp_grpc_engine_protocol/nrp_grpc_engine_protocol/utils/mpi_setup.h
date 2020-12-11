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

#ifndef MPI_SETUP_H
#define MPI_SETUP_H

#include <memory>
#include <mpi.h>

/*!
 * \brief Singleton to setup MPI and MPI4Py before first use
 */
class MPISetup
{
	public:
		/*!
		 * \brief Get Singleton Instance
		 * \return Returns nullptr if not yet initalized
		 */
		static MPISetup *getInstance();

		/*!
		 * \brief Reset Singleton Instance
		 * \param argc MPI_Init argument
		 * \param argv MPI_Init argument
		 */
		static MPISetup *resetInstance(int argc, char **argv);

		/*!
		 * \brief Initialize MPI if not yet done
		 * \param argc MPI_Init argument
		 * \param argv MPI_Init argument
		 * \param sendPID Should the PID be sent to the parent after running MPI_INIT?
		 * This is only required in an engine spawned via the launch command "MPI"
		 */
		static MPISetup *initializeOnce(int argc, char **argv, bool sendPID = false);

		/*!
		 * \brief Finalizes MPI if currently initialized
		 */
		static void finalize();

		/*!
		 * \brief Get error string for an MPI Error Code
		 * \param MPIErrorCode Error Code
		 * \return Returns string
		 */
		static std::string getErrorString(int MPIErrorCode);

		/*!
		 * \brief Get MPI Communicator to interacti with parent process
		 * \return If this process does not have an MPI-ready parent, return MPI_COMM_NULL
		 */
		static MPI_Comm getParentComm();

		/*!
		 * \brief Send current PID process
		 * \param comm Communicator to use. Default is parent process communicator
		 * \param tag MPI Message tag
		 */
		void sendPID(MPI_Comm comm = MPISetup::getParentComm(), int tag = 0);

		/*!
		 * \brief Receive PID process
		 * \param comm MPI Communicator to use
		 * \param tag MPI Message tag
		 */
		pid_t recvPID(MPI_Comm comm, int tag);

		/*!
		 * \brief Finalizes MPI
		 */
		~MPISetup();

		// Delete for singleton
		MPISetup(const MPISetup&) = delete;
		MPISetup(MPISetup&&) = delete;

		MPISetup &operator=(const MPISetup&) = delete;
		MPISetup &operator=(MPISetup&&) = delete;

	private:
		/*!
		 * \brief Initializes MPI
		 * \param argc
		 * \param argv
		 */
		MPISetup(int argc, char **argv);

		/*!
		 * \brief Singleton instance
		 */
		static std::unique_ptr<MPISetup> _instance;
};

#endif // MPI_SETUP_H
