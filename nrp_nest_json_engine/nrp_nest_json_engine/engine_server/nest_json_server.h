#ifndef NEST_JSON_SERVER_H
#define NEST_JSON_SERVER_H

#include "nrp_json_engine_protocol/engine_server/engine_json_server.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_nest_json_engine/engine_server/nest_engine_device_controller.h"

#include "nrp_nest_json_engine/config/nest_config.h"

#include "nrp_nest_json_engine/devices/nest_device.h"

#include "nrp_nest_json_engine/python/create_device_class.h"


#include <boost/python.hpp>

class NestJSONServer
        : public EngineJSONServer
{
	public:
		NestJSONServer(const std::string &serverAddress, boost::python::dict globals, boost::python::object locals);
		NestJSONServer(const std::string &serverAddress, const std::string &engineName, const std::string &registrationAddress, boost::python::dict globals, boost::python::object locals);
		virtual ~NestJSONServer() override;

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

		virtual float runLoopStep(float timeStep) override;
		virtual nlohmann::json initialize(const nlohmann::json &data, EngineJSONServer::lock_t &deviceLock) override;
		virtual nlohmann::json shutdown(const nlohmann::json &data) override;

	private:
		/*!
		 * \brief Init Flag. Set to true once the server has executed the initialize function
		 */
		bool _initRunFlag = false;

		/*!
		 * \brief Shutdown Flag. Set to true once the shutdown signal has been received
		 */
		bool _shutdownFlag = false;

		/*!
		 * \brief NEST Preparation Flag. Set to true once nest.Prepare() was run and back to false after nest.Cleanup() was run
		 */
		bool _nestPreparedFlag = false;

		/*!
		 * \brief Global Python variables
		 */
		boost::python::dict _pyGlobals;

		/*!
		 * \brief Local Python variables
		 */
		boost::python::object _pyLocals;

		/*!
		 * \brief Nest object
		 */
		boost::python::dict _pyNest;

		/*!
		 * \brief NRP Nest object
		 */
		boost::python::dict _pyNRPNest;

		/*!
		 * \brief NEST Device Mapping (from Device Name to NEST NodeCollection)
		 */
		boost::python::dict _devMap;

		/*!
		 * \brief List of device ptrs. Used to manage controller deletion
		 */
		std::list<EngineDeviceControllerInterface<nlohmann::json>::shared_ptr> _deviceControllerPtrs;

		/*!
		 *	\brief GIL Lock state
		 */
		PyGILState_STATE _pyGILState;

		/*!
		 * \brief Converts seconds to milliseconds
		 * \param sec Seconds
		 * \return Returns milliseconds
		 */
		static constexpr float convertSecToMill(const float sec);

		/*!
		 * \brief Converts milliseconds to seconds
		 * \param milsec Milliseconds
		 * \return Returns seconds
		 */
		static constexpr float convertMillToSec(const float millsec);

		/*!
		 * \brief Creates an error message to be returned to the main NRP process
		 * \param errMsg Error text
		 * \return Returns a JSON object containing the error text as well as a failure flag
		 */
		static nlohmann::json formatInitErrorMessage(const std::string &errMsg);

		nlohmann::json getDeviceData(const nlohmann::json &reqData) override;

		nlohmann::json setDeviceData(const nlohmann::json &reqData) override;
};

#endif // NEST_JSON_SERVER_H
