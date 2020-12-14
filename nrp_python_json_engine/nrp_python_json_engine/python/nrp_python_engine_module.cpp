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

#include <boost/python.hpp>

#include "nrp_general_library/config/cmake_constants.h"
#include "nrp_python_json_engine/config/cmake_constants.h"
#include "nrp_python_json_engine/engine_server/python_json_server.h"
#include "nrp_python_json_engine/python/py_engine_script.h"
#include "nrp_python_json_engine/python/py_engine_script_wrapper.h"


namespace python = boost::python;

/*!
 * \brief Calls PythonJSONServer::registerScript() without returning a value
 */
void PyServerRegistration(python::object script)
{	PythonJSONServer::registerScript(script);	}

/*!
 * \brief Decorator for engine script class.
 * User can decorate their EngineScript class with this to register it with the server
 */
struct PyRegisterEngineDecorator
{
	/*!
	 * \brief __call__() function
	 * \param script Class derived from EngineScript
	 * \return Returns ref to PyEngineScript
	 */
	PyEngineScript &pyCall(python::object script)
	{	return *(PythonJSONServer::registerScript(script));	}
};


BOOST_PYTHON_MODULE(NRP_PYTHON_ENGINE_MODULE)
{
	//python::class_<PyEngineScript>("__IntEngineScript", python::init<>());

	// Import General NRP Python Module
	python::import(PYTHON_MODULE_NAME_STR);

	python::class_<SimulationTime>("SimulationTime")
		.def("count", &SimulationTime::count)
	;

	// Engine Script Class. Used by users to define script that should be executed
	python::class_<PyEngineScriptWrapper, boost::noncopyable>("EngineScript", python::init<>())
	        .def("initialize", &PyEngineScriptWrapper::initialize, &PyEngineScriptWrapper::defaultInitialize)
	        .def("runLoop", python::pure_virtual(&PyEngineScriptWrapper::runLoop))
	        .def("shutdown", &PyEngineScriptWrapper::shutdown, &PyEngineScriptWrapper::defaultShutdown)
	        .add_property("_time", &PyEngineScript::simTime)
	        .def("_registerDevice", &PyEngineScript::registerDevice)
	        .def("_getDevice", &PyEngineScript::getDevice, python::return_value_policy<python::copy_non_const_reference>())
	        .def("_setDevice", &PyEngineScript::setDevice);

	python::register_ptr_to_python<PyEngineScriptSharedPtr>();
	python::register_ptr_to_python<PyEngineScriptConstSharedPtr>();

	// Engine Registration functions
	python::class_<PyRegisterEngineDecorator, boost::noncopyable>("RegisterEngine", python::init<>())
	        .def("__call__", &PyRegisterEngineDecorator::pyCall, python::return_internal_reference<>());
}
