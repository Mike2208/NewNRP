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

#ifndef PYTHON_JSON_SERVER_H
#define PYTHON_JSON_SERVER_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_server.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_python_json_engine/config/python_config.h"
#include "nrp_python_json_engine/engine_server/python_engine_json_device_controller.h"

#include <boost/python.hpp>

class PyEngineScript;

class PythonJSONServer
        : public EngineJSONServer
{
	public:
		PythonJSONServer(const std::string &serverAddress, boost::python::dict globals, boost::python::object locals);
		PythonJSONServer(const std::string &serverAddress, const std::string &engineName, const std::string &registrationAddress, boost::python::dict globals, boost::python::object locals);
		virtual ~PythonJSONServer() override = default;

		/*!
		 * \brief Has the initialization been executed?
		 * \return Returns true once the initialize function has been run once
		 */
		bool initRunFlag() const;

		/*!
		 * \brief Has a shutdown command been received?
		 * \return Returns true if a shutdown command has been received
		 */
		bool shutdownFlag() const;

		virtual SimulationTime runLoopStep(SimulationTime timeStep) override;
		virtual nlohmann::json initialize(const nlohmann::json &data, EngineJSONServer::lock_t &deviceLock) override;
		virtual nlohmann::json shutdown(const nlohmann::json &data) override;

		/*!
		 * \brief Register pointer to python script
		 * \param pythonScript Pointer to PyEngineScript
		 * \return Returns ptr to PyEngineScript of pythonScript
		 */
		static PyEngineScript *registerScript(const boost::python::object &pythonScript);

	private:
		/*!
		 * \brief Ptr to current PythonJSONServer.
		 * When registerScript() is called, it will use this ptr to determine the class where the pythonScript will be stored
		 */
		static PythonJSONServer *_registrationPyServer;

		/*!
		 * \brief Init Flag. Set to true once the server has executed the initialize function
		 */
		bool _initRunFlag = false;

		/*!
		 * \brief Shutdown Flag. Set to true once the shutdown signal has been received
		 */
		bool _shutdownFlag = false;

		/*!
		 * \brief Global Python variables
		 */
		boost::python::dict _pyGlobals;

		/*!
		 * \brief Local Python variables
		 */
		boost::python::object _pyLocals;

		/*!
		 * \brief Python script to execute
		 */
		boost::python::object _pyEngineScript;

		/*!
		 * \brief List of device ptrs. Used to manage controller deletion
		 */
		std::list<EngineDeviceControllerInterface<nlohmann::json>::shared_ptr> _deviceControllerPtrs;

		/*!
		 *	\brief GIL Lock state
		 */
		PyGILState_STATE _pyGILState;

		/*!
		 * \brief Creates an error message to be returned to the main NRP process
		 * \param errMsg Error text
		 * \return Returns a JSON object containing the error text as well as a failure flag
		 */
		static nlohmann::json formatInitErrorMessage(const std::string &errMsg);

		nlohmann::json getDeviceData(const nlohmann::json &reqData) override;

		nlohmann::json setDeviceData(const nlohmann::json &reqData) override;

};

#endif // PYTHON_JSON_SERVER_H
