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

#include "nrp_python_json_engine/engine_server/python_json_server.h"

#include "nrp_general_library/utils/python_error_handler.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_python_json_engine/config/cmake_constants.h"
#include "nrp_python_json_engine/engine_server/python_engine_json_device_controller.h"
#include "nrp_python_json_engine/python/py_engine_script.h"

#include <fstream>

namespace python = boost::python;

PythonJSONServer *PythonJSONServer::_registrationPyServer = nullptr;

PythonJSONServer::PythonJSONServer(const std::string &serverAddress, python::dict globals, python::object locals)
    : EngineJSONServer(serverAddress),
      _pyGlobals(globals),
      _pyLocals(locals)
{}

PythonJSONServer::PythonJSONServer(const std::string &serverAddress, const std::string &engineName, const std::string &registrationAddress, python::dict globals, boost::python::object locals)
    : EngineJSONServer(serverAddress, engineName, registrationAddress),
      _pyGlobals(globals),
      _pyLocals(locals)
{}

bool PythonJSONServer::initRunFlag() const
{
	return this->_initRunFlag;
}

bool PythonJSONServer::shutdownFlag() const
{
	return this->_shutdownFlag;
}

SimulationTime PythonJSONServer::runLoopStep(SimulationTime timestep)
{
	PythonGILLock lock(this->_pyGILState, true);

	try
	{
		PyEngineScript &script = python::extract<PyEngineScript&>(this->_pyEngineScript);
		return script.runLoop(timestep);
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server
		throw NRPExceptionNonRecoverable(handle_pyerror());
	}
}

nlohmann::json PythonJSONServer::initialize(const nlohmann::json &data, EngineJSONServer::lock_t&)
{
	PythonGILLock lock(this->_pyGILState, true);
	try
	{
		// Load python
		this->_pyGlobals.update(python::dict(python::import(NRP_PYTHON_ENGINE_MODULE_STR).attr("__dict__")));
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server without setting the initRunFlag
		return this->formatInitErrorMessage(handle_pyerror());
	}

	// Read received configuration
	const PythonConfig config(data.at(PythonConfig::ConfigType.m_data));

	// Read python script file if present
	const std::filesystem::path fileName = config.pythonFileName();
	if(fileName.empty())
	{
		const auto errMsg = "No python filename given. Aborting...";
		NRPLogger::SPDErrLogDefault(errMsg);
		return this->formatInitErrorMessage(errMsg);
	}

	if(!std::filesystem::exists(fileName))
	{
		const auto errMsg = "Could not find init file " + std::string(fileName);
		NRPLogger::SPDErrLogDefault(errMsg);
		return this->formatInitErrorMessage(errMsg);
	}

	// Prepare registration
	PythonJSONServer::_registrationPyServer = this;

	// Read python file
	try
	{
		python::exec_file(fileName.c_str(), this->_pyGlobals, this->_pyLocals);
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server without setting the initRunFlag
		const auto msg = handle_pyerror();
		NRPLogger::SPDErrLogDefault(msg);
		return this->formatInitErrorMessage(msg);
	}

	// Check that executed file also
	if(PythonJSONServer::_registrationPyServer != nullptr)
	{
		PythonJSONServer::_registrationPyServer = nullptr;
		const auto errMsg = "Failed to initialize Python server. Given python file \"" + std::string(fileName) + "\" does not register a script";
		NRPLogger::SPDErrLogDefault(errMsg);
		return this->formatInitErrorMessage(errMsg);
	}

	// Run user-defined initialize function
	try
	{
		PyEngineScript &script = python::extract<PyEngineScript&>(this->_pyEngineScript);
		script.initialize();
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server without setting the initRunFlag
		const auto msg = handle_pyerror();
		NRPLogger::SPDErrLogDefault(msg);
		return this->formatInitErrorMessage(msg);

	}

	// Init has run once
	this->_initRunFlag = true;

	// Return success and parsed devmap
	return nlohmann::json({{PythonConfig::InitFileExecStatus, true}});
}

nlohmann::json PythonJSONServer::shutdown(const nlohmann::json &)
{
	PythonGILLock lock(this->_pyGILState, true);

	this->_shutdownFlag = true;

	if(this->_initRunFlag)
	{
		// Run user-defined Shutdown fcn
		try
		{
			PyEngineScript &script = python::extract<PyEngineScript&>(this->_pyEngineScript);
			script.shutdown();
		}
		catch(python::error_already_set &)
		{
			// If an error occured, return the message to the NRP server
			throw NRPExceptionNonRecoverable(handle_pyerror());
		}
	}

	// Remove device controllers
	this->clearRegisteredDevices();
	this->_deviceControllerPtrs.clear();

	return nlohmann::json();
}

PyEngineScript *PythonJSONServer::registerScript(const boost::python::object &pythonScript)
{
	assert(PythonJSONServer::_registrationPyServer != nullptr);

	// Register script with server
	PythonJSONServer::_registrationPyServer->_pyEngineScript = pythonScript();

	// Register server with script
	PyEngineScript &script = boost::python::extract<PyEngineScript&>(PythonJSONServer::_registrationPyServer->_pyEngineScript);
	script.setPythonJSONServer(PythonJSONServer::_registrationPyServer);

	PythonJSONServer::_registrationPyServer = nullptr;

	return &script;
}

nlohmann::json PythonJSONServer::formatInitErrorMessage(const std::string &errMsg)
{
	return nlohmann::json({{PythonConfig::InitFileExecStatus, 0}, {PythonConfig::InitFileErrorMsg, errMsg}});
}

nlohmann::json PythonJSONServer::getDeviceData(const nlohmann::json &reqData)
{
	PythonGILLock lock(this->_pyGILState, true);
	return this->EngineJSONServer::getDeviceData(reqData);
}

nlohmann::json PythonJSONServer::setDeviceData(const nlohmann::json &reqData)
{
	PythonGILLock lock(this->_pyGILState, true);
	return this->EngineJSONServer::setDeviceData(reqData);
}
