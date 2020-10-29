#ifndef NEST_SERVER_EXECUTABLE_H
#define NEST_SERVER_EXECUTABLE_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_opts_parser.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_nest_server_engine/engine_server/nest_server.h"

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

		NestServer _server;

		NestServerExecutable(int argc, char *argv[]);

		/*!
		 * \brief Handle a call from SIGTERM to terminate server
		 * \param signal Signal to handle
		 */
		static void handleSIGTERM(int signal);
};

#endif // NEST_SERVER_EXECUTABLE_H
