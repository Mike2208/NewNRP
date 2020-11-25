#include "example_engine_server_executable/example_engine_server_executable.h"

#include "nrp_general_library/utils/spdlog_setup.h"

#include <boost/python.hpp>
#include <csignal>

using namespace boost;

std::unique_ptr<ExampleEngineServerExecutable> ExampleEngineServerExecutable::_instance = nullptr;

ExampleEngineServerExecutable::ExampleEngineServerExecutable(int argc, char *argv[])
    : _res(EngineJSONOptsParser::parseOpts(argc, argv, EngineJSONOptsParser::createOptionParser())),
      _server(this->_res[EngineJSONConfigConst::EngineServerAddrArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineNameArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineRegistrationServerAddrArg.data()].as<std::string>())
{
	// Register function to handle SIGTERM events
	signal(SIGTERM, &ExampleEngineServerExecutable::handleSIGTERM);
}

ExampleEngineServerExecutable &ExampleEngineServerExecutable::getInstance()
{
	return *(ExampleEngineServerExecutable::_instance.get());
}

ExampleEngineServerExecutable &ExampleEngineServerExecutable::resetInstance(int argc, char *argv[])
{
	ExampleEngineServerExecutable::_instance.reset(new ExampleEngineServerExecutable(argc, argv));

	return ExampleEngineServerExecutable::getInstance();
}

void ExampleEngineServerExecutable::shutdown()
{
	ExampleEngineServerExecutable::_instance.reset();
}

void ExampleEngineServerExecutable::startServerAsync()
{
	return this->_server.startServerAsync();
}

bool ExampleEngineServerExecutable::serverRunning() const
{
	return this->_server.isServerRunning();
}

void ExampleEngineServerExecutable::waitForInit()
{
	// Wait for initialization function to be run. Stop if shutdown command was received
	while(!this->_server.initRunFlag() ||
		  this->_server.shutdownFlag())
	{
		sleep(0);
	}
}

int ExampleEngineServerExecutable::run()
{
	// Continue handling requests until a shutdown command has been received
	while(!this->_server.shutdownFlag())
	{
		sleep(1);
	}

	// Shutdown server
	this->_server.shutdownServer();
	return 0;
}

void ExampleEngineServerExecutable::handleSIGTERM(int signal)
{
	if(signal == SIGTERM)
	{
		// Explicitly call destructor, then exit program
		try
		{
			ExampleEngineServerExecutable::shutdown();
		}
		catch(std::exception &e)
		{
			NRPException::logCreate(e, "NRP Nest Server shutdown failed after receiving SIGTERM signal");
		}

		SPDLogSetup::shutdownDefault();
		exit(signal);
	}
}
