#ifndef PYTHON_SERVER_EXECUTABLE_H
#define PYTHON_SERVER_EXECUTABLE_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_opts_parser.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_python_json_engine/engine_server/python_json_server.h"

/*!
 * \brief Executes an instance of PYTHON along with a JSON server
 */
class PythonServerExecutable
{
	public:
		~PythonServerExecutable();

		// Delete move and copy operators. This ensures this class is a singleton
		PythonServerExecutable(const PythonServerExecutable &) = delete;
		PythonServerExecutable(PythonServerExecutable &&) = delete;

		PythonServerExecutable &operator=(const PythonServerExecutable &) = delete;
		PythonServerExecutable &operator=(PythonServerExecutable &&) = delete;

		/*!
		 * \brief Get singleton instance of executable
		 * \return Returns singleton instance
		 */
		static PythonServerExecutable &getInstance();

		/*!
		 * \brief Reset singletong instance with given start parameters
		 * \param argc
		 * \param argv
		 * \return Returns singleton instance
		 */
		static PythonServerExecutable &resetInstance(int argc, char *argv[]);

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
		 * \brief Start the python server loop. Executes commands sent from the NRP
		 * \return Returns result of loop
		 */
		int run();

		/*!
		 * \brief Get Python Interpreter
		 */
		PythonInterpreterState &pyState();

	private:
		static std::unique_ptr<PythonServerExecutable> _instance;

		cxxopts::ParseResult _res;

		PythonInterpreterState _pyInterp;

		PythonJSONServer _server;

		PythonServerExecutable(int argc, char *argv[]);

		/*!
		 * \brief Handle a call from SIGTERM to terminate server
		 * \param signal Signal to handle
		 */
		static void handleSIGTERM(int signal);
};

#endif // PYTHON_SERVER_EXECUTABLE_H
