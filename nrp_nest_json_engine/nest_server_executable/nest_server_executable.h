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

#ifndef NEST_SERVER_EXECUTABLE_H
#define NEST_SERVER_EXECUTABLE_H

#include "nrp_json_engine_protocol/engine_server/engine_json_opts_parser.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_nest_json_engine/engine_server/nest_json_server.h"

/*!
 * \brief Executes an instance of NEST along with a JSON server
 */
class NestServerExecutable
{
	public:
		~NestServerExecutable();

		// Delete move and copy operators. This ensures this class is a singleton
		NestServerExecutable(const NestServerExecutable &) = delete;
		NestServerExecutable(NestServerExecutable &&) = delete;

		NestServerExecutable &operator=(const NestServerExecutable &) = delete;
		NestServerExecutable &operator=(NestServerExecutable &&) = delete;

		/*!
		 * \brief Get singleton instance of executable
		 * \return Returns singleton instance
		 */
		static NestServerExecutable &getInstance();

		/*!
		 * \brief Reset singletong instance with given start parameters
		 * \param argc
		 * \param argv
		 * \return Returns singleton instance
		 */
		static NestServerExecutable &resetInstance(int argc, char *argv[]);

		/*!
		 * \brief Shutdown the server. Must be executed before the Python Environment is finalized to prevent segfaults
		 */
		static void shutdown();

		/*!
		 * \brief Start the server
		 */
		void startServerAsync();

		/*!
		 * \brief Is the server running
		 * \return Returns true if the server is running, false otherwise
		 */
		bool serverRunning() const;

		/*!
		 * \brief Wait for initialization signal from the NRP
		 */
		void waitForInit();

		/*!
		 * \brief Start the nest server loop. Executes commands sent from the NRP
		 * \return Returns result of loop
		 */
		int run();

		/*!
		 * \brief Get Python Interpreter
		 */
		PythonInterpreterState &pyState();

	private:
		static std::unique_ptr<NestServerExecutable> _instance;

		cxxopts::ParseResult _res;

		PythonInterpreterState _pyInterp;

		NestJSONServer _server;

		NestServerExecutable(int argc, char *argv[]);

		/*!
		 * \brief Handle a call from SIGTERM to terminate server
		 * \param signal Signal to handle
		 */
		static void handleSIGTERM(int signal);
};

#endif // NEST_SERVER_EXECUTABLE_H
