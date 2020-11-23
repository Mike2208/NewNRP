#include "python_server_executable/python_server_executable.h"

#include <boost/python.hpp>

int main(int argc, char *argv[])
{
	// Load the Python server
	auto &server = PythonServerExecutable::resetInstance(argc, argv);

	// Start the server in separate thread
	server.startServerAsync();

	// Wait for Initialization call
	server.waitForInit();

	// Run server
	return server.run();
}
