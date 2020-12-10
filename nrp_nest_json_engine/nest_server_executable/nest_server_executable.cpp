//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nest_server_executable/nest_server_executable.h"

#include "nrp_general_library/utils/spdlog_setup.h"

#include <boost/python.hpp>
#include <csignal>

using namespace boost;

std::unique_ptr<NestServerExecutable> NestServerExecutable::_instance = nullptr;

NestServerExecutable::~NestServerExecutable()
{
	this->_server.shutdownServer();
	this->_pyInterp.endAllowThreads();
}

NestServerExecutable::NestServerExecutable(int argc, char *argv[])
    : _res(EngineJSONOptsParser::parseOpts(argc, argv, EngineJSONOptsParser::createOptionParser(true))),
	  _pyInterp(argc, argv),
      _server(this->_res[EngineJSONConfigConst::EngineServerAddrArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineNameArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineRegistrationServerAddrArg.data()].as<std::string>(),
              python::dict(python::import("__main__").attr("__dict__")), python::dict(python::import("__main__").attr("__dict__")))
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
		catch(std::exception &e)
		{
			NRPException::logCreate(e, "NRP Nest Server shutdown failed after receiving SIGTERM signal");
		}

		SPDLogSetup::shutdownDefault();
		exit(signal);
	}
}
