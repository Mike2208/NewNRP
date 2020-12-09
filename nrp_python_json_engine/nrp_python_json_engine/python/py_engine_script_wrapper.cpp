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
