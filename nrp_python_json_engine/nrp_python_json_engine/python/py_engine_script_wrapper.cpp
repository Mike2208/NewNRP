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

#include "nrp_python_json_engine/python/py_engine_script_wrapper.h"

#include "nrp_python_device/python_module.h"

namespace python = boost::python;

void PyEngineScriptWrapper::initialize()
{
	if(python::override initializeFcn = this->get_override("initialize"))
		initializeFcn();

	return PyEngineScript::initialize();
}

void PyEngineScriptWrapper::defaultInitialize()
{	return PyEngineScript::initialize();	}

void PyEngineScriptWrapper::runLoopFcn(SimulationTime timestep)
{	this->get_override("runLoop")(timestep);	}

void PyEngineScriptWrapper::shutdown()
{
	if(python::override shutdownFcn = this->get_override("shutdown"))
		shutdownFcn();

	return PyEngineScript::shutdown();
}

void PyEngineScriptWrapper::defaultShutdown()
{	return PyEngineScript::shutdown();	}
