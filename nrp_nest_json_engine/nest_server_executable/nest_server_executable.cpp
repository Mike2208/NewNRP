#include "nest_server_executable/nest_server_executable.h"

#include <boost/python.hpp>
#include <csignal>

using namespace boost;

std::unique_ptr<NestServerExecutable> NestServerExecutable::_instance = nullptr;

NestServerExecutable::~NestServerExecutable()
{
	this->_pyInterp.endAllowThreads();
}

NestServerExecutable::NestServerExecutable(int argc, char *argv[])
    : _res(EngineJSONOptsParser::parseOpts(argc, argv, EngineJSONOptsParser::createOptionParser())),
	  _pyInterp(argc, argv),
      _server(this->_res[EngineJSONConfigConst::EngineServerAddrArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineNameArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineRegistrationServerAddrArg.data()].as<std::string>(),
              python::dict(python::import("__main__").attr("__dict__")), python::dict())
{
	// Register function to handle SIGTERM events
	signal(SIGTERM, &NestServerExecutable::handleSIGTERM);
}

NestServerExecutable &NestServerExecutable::getInstance()
{
	return *(NestServerExecutable::_instance.get());
}

NestServerExecutable &NestServerExecutable::resetInstance(int argc, char *argv[])
{
	NestServerExecutable::_instance.reset(new NestServerExecutable(argc, argv));

	return NestServerExecutable::getInstance();
}

void NestServerExecutable::shutdown()
{
	if(NestServerExecutable::_instance != nullptr)
		NestServerExecutable::_instance->_pyInterp.endAllowThreads();

	NestServerExecutable::_instance.reset();
}

void NestServerExecutable::startServerAsync()
{
	// Allow threads after starting server
	this->_pyInterp.allowThreads();

	return this->_server.startServerAsync();
}

bool NestServerExecutable::serverRunning() const
{
	return this->_server.isServerRunning();
}

void NestServerExecutable::waitForInit()
{
	// Wait for initialization function to be run. Stop if shutdown command was received
	while(!this->_server.initRunFlag() ||
		  this->_server.shutdownFlag())
	{
		sleep(0);
	}
}

int NestServerExecutable::run()
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

PythonInterpreterState &NestServerExecutable::pyState()
{
	return this->_pyInterp;
}

void NestServerExecutable::handleSIGTERM(int signal)
{
	if(signal == SIGTERM)
	{
		// Explicitly call destructor, then exit program
		try
		{
			NestServerExecutable::shutdown();
		}
		catch(const std::exception &e)
		{
			std::cerr << e.what();
		}

		exit(signal);
	}
}
