#include "nrp_python_device_interface/engine_server/python_json_server.h"

#include "nrp_general_library/utils/python_error_handler.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_python_device_interface/config/cmake_constants.h"
#include "nrp_python_device_interface/engine_server/python_engine_json_device_controller.h"
#include "nrp_python_device_interface/python/py_engine_script.h"

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

float PythonJSONServer::runLoopStep(float timestep)
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
	const auto &fileName = config.pythonFileName();
	if(fileName.empty())
	{
		const auto errMsg = "No python filename given. Aborting...";
		std::cerr << errMsg << std::endl;
		return this->formatInitErrorMessage(errMsg);
	}

	if(!std::filesystem::exists(fileName))
	{
		const auto errMsg = "Could not find init file " + fileName;
		std::cerr << errMsg << std::endl;
		return this->formatInitErrorMessage(errMsg);
	}

	// Prepare registration
	PythonJSONServer::_registrationPyServer = this;

	// Read python file
	try
	{
		python::exec_file(python::str(fileName), this->_pyGlobals, this->_pyLocals);
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server without setting the initRunFlag
		if (PyErr_Occurred())
		{
			const auto msg = handle_pyerror();
			PyErr_Clear();

			std::cerr << msg;
			return this->formatInitErrorMessage(msg);
		}
	}

	// Check that executed file also
	if(PythonJSONServer::_registrationPyServer != nullptr)
	{
		PythonJSONServer::_registrationPyServer = nullptr;
		const auto errMsg = "Failed to initialize Python server. Given python file \"" + fileName + "\" does not register a script";
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
		if (PyErr_Occurred())
		{
			const auto msg = handle_pyerror();
			PyErr_Clear();

			std::cerr << msg << std::endl;
			return this->formatInitErrorMessage(msg);
		}
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
