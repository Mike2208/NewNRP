#ifndef EXAMPLE_ENGINE_SERVER_EXECUTABLE_H
#define EXAMPLE_ENGINE_SERVER_EXECUTABLE_H

#include "nrp_json_engine_protocol/engine_server/engine_json_opts_parser.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_example_engine/engine_server/example_engine_server.h"

class ExampleEngineServerExecutable
{
	public:
		~ExampleEngineServerExecutable() = default;

		// Delete move and copy operators. This ensures this class is a singleton
		ExampleEngineServerExecutable(const ExampleEngineServerExecutable &) = delete;
		ExampleEngineServerExecutable(ExampleEngineServerExecutable &&) = delete;

		ExampleEngineServerExecutable &operator=(const ExampleEngineServerExecutable &) = delete;
		ExampleEngineServerExecutable &operator=(ExampleEngineServerExecutable &&) = delete;

		static ExampleEngineServerExecutable &getInstance();
		static ExampleEngineServerExecutable &resetInstance(int argc, char *argv[]);

		static void shutdown();

		void startServerAsync();
		bool serverRunning() const;

		void waitForInit();
		int run();

	private:
		static std::unique_ptr<ExampleEngineServerExecutable> _instance;

		cxxopts::ParseResult _res;

		ExampleEngineServer _server;

		ExampleEngineServerExecutable(int argc, char *argv[]);

		static void handleSIGTERM(int signal);
};

#endif // EXAMPLE_ENGINE_SERVER_EXECUTABLE_H
