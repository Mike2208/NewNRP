#ifndef PY_ENGINE_SCRIPT_H
#define PY_ENGINE_SCRIPT_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/utils/ptr_templates.h"
#include "nrp_python_json_engine/engine_server/python_json_server.h"

#include <boost/python.hpp>
#include <map>

/*!
 * \brief C++ class to interface with user-defined Python script class
 */
class PyEngineScript
        : public PtrTemplates<PyEngineScript>
{
	public:
		PyEngineScript() = default;
		virtual ~PyEngineScript();

		/*!
		 * \brief Initialization function. Called at start of simulation
		 */
		virtual void initialize();

		/*!
		 * \brief Runs main script loop and updates _time
		 * \param timestep Time (in seconds) to run the loop
		 * \return Returns total simulation time of this engine
		 */
		inline float runLoop(float timestep)
		{
			this->runLoopFcn(timestep);
			this->_time += timestep;

			return this->_time;
		}

		/*!
		 * \brief Shutdown function. Called at end of simulation
		 */
		virtual void shutdown();

		/*!
		 * \brief Get simulation time of this engine
		 */
		float simTime() const;

		/*!
		 * \brief Register device
		 * \param deviceName Name of device
		 */
		void registerDevice(std::string deviceName);

		/*!
		 * \brief Get Device Data
		 * \param deviceName Name of device
		 * \return Returns device data
		 */
		boost::python::object &getDevice(const std::string &deviceName);

		/*!
		 * \brief Set Device Data
		 * \param deviceName Name of device
		 * \param data Data to store in device
		 */
		void setDevice(const std::string &deviceName, boost::python::object data);

		/*!
		 * \brief Save ptr to PythonJSONServer instance that owns this script
		 * \param pServer Pointer to PythonJSONServer
		 */
		void setPythonJSONServer(PythonJSONServer *pServer);

	protected:
		/*!
		 * \brief Main script loop. Will run for timestep seconds
		 * \param timestep Time (in seconds) to run the loop
		 */
		virtual void runLoopFcn(float timestep) = 0;

	private:
		/*!
		 * \brief Engine Time
		 */
		float _time = 0;

		/*!
		 * \brief Ptr to PythonJSONServer instance that owns this script
		 */
		PythonJSONServer *_pServer = nullptr;

		/*!
		 * \brief Device Controllers
		 */
		std::list<EngineJSONDeviceController::shared_ptr> _deviceControllers;

		/*!
		 * \brief Map from keyword to device data
		 */
		std::map<std::string, boost::python::object*> _nameDeviceMap;
};

using PyEngineScriptSharedPtr = PyEngineScript::shared_ptr;
using PyEngineScriptConstSharedPtr = PyEngineScript::const_shared_ptr;

#endif // PY_ENGINE_SCRIPT_H
