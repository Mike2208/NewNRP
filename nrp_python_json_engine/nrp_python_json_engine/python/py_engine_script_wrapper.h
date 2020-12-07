#ifndef PY_ENGINE_SCRIPT_WRAPPER_H
#define PY_ENGINE_SCRIPT_WRAPPER_H

#include "nrp_python_json_engine/python/py_engine_script.h"

/*!
 * \brief Wrapper around PyEngineScript.
 * Used to make derived python classes available from C++
 */
struct PyEngineScriptWrapper
        : PyEngineScript, boost::python::wrapper<PyEngineScript>
{
	void initialize() override;

	void defaultInitialize();

	void runLoopFcn(SimulationTime timestep) override;

	void shutdown() override;

	void defaultShutdown();
};

#endif // PY_ENGINE_SCRIPT_WRAPPER_H
