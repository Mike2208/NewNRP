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

#include "python_server_executable/python_server_executable.h"

#include "nrp_general_library/utils/spdlog_setup.h"

#include <boost/python.hpp>
#include <csignal>

using namespace boost;

std::unique_ptr<PythonServerExecutable> PythonServerExecutable::_instance = nullptr;

PythonServerExecutable::~PythonServerExecutable()
{
	this->_pyInterp.endAllowThreads();
}

PythonServerExecutable::PythonServerExecutable(int argc, char *argv[])
    : _res(EngineJSONOptsParser::parseOpts(argc, argv, EngineJSONOptsParser::createOptionParser())),
	  _pyInterp(argc, argv),
      _server(this->_res[EngineJSONConfigConst::EngineServerAddrArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineNameArg.data()].as<std::string>(),
              this->_res[EngineJSONConfigConst::EngineRegistrationServerAddrArg.data()].as<std::string>(),
              python::dict(python::import("__main__").attr("__dict__")), python::dict())
{
	// Register function to handle SIGTERM events
	signal(SIGTERM, &PythonServerExecutable::handleSIGTERM);
}

PythonServerExecutable &PythonServerExecutable::getInstance()
{
	return *(PythonServerExecutable::_instance.get());
}

PythonServerExecutable &PythonServerExecutable::resetInstance(int argc, char *argv[])
{
	PythonServerExecutable::_instance.reset(new PythonServerExecutable(argc, argv));

	return PythonServerExecutable::getInstance();
}

void PythonServerExecutable::shutdown()
{
	if(PythonServerExecutable::_instance != nullptr)
		PythonServerExecutable::_instance->_pyInterp.endAllowThreads();

	PythonServerExecutable::_instance.reset();
}

void PythonServerExecutable::startServerAsync()
{
	// Allow threads after starting server
	this->_pyInterp.allowThreads();

	return this->_server.startServerAsync();
}

bool PythonServerExecutable::serverRunning() const
{
	return this->_server.isServerRunning();
}

void PythonServerExecutable::waitForInit()
{
	// Wait for initialization function to be run. Stop if shutdown command was received
	while(!this->_server.initRunFlag() ||
		  this->_server.shutdownFlag())
	{
		sleep(0);
	}
}

int PythonServerExecutable::run()
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

PythonInterpreterState &PythonServerExecutable::pyState()
{
	return this->_pyInterp;
}

void PythonServerExecutable::handleSIGTERM(int signal)
{
	if(signal == SIGTERM)
	{
		// Explicitly call destructor, then exit program
		try
		{
			PythonServerExecutable::shutdown();
		}
		catch(std::exception &e)
		{
			NRPException::logCreate(e, "NRP Server shutdown failed after receiving SIGTERM signal");
		}

		SPDLogSetup::shutdownDefault();
		exit(signal);
	}
}
