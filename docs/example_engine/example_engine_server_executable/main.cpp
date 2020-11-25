#include "example_engine_server_executable/example_engine_server_executable.h"

int main(int argc, char *argv[])
{
	// Load the Nest server
	auto &server = ExampleEngineServerExecutable::resetInstance(argc, argv);

	// Start the server in separate thread
	server.startServerAsync();

	// Wait for Initialization call
	server.waitForInit();

	// Run server
	return server.run();
}
