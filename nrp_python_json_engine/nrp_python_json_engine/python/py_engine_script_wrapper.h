/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

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
