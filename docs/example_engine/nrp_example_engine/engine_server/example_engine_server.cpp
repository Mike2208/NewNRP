#include "nrp_example_engine/engine_server/example_engine_server.h"

ExampleEngineServer::ExampleEngineServer(const std::string &engineName)
    : _engineName(engineName)
{}

bool ExampleEngineServer::initRunFlag() const
{
	return this->_initRunFlag;
}

bool ExampleEngineServer::shutdownFlag() const
{
	return this->_shutdownFlag;
}

