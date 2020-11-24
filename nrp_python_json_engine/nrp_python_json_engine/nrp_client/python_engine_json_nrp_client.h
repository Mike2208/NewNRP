#ifndef PYTHON_ENGINE_JSON_NRP_CLIENT_H
#define PYTHON_ENGINE_JSON_NRP_CLIENT_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/nrp_client/engine_json_nrp_client.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/plugin_system/plugin.h"

#include "nrp_python_json_engine/config/python_config.h"

#include <unistd.h>

/*!
 * \brief NRP - Python Communicator on the NRP side. Converts DeviceInterface classes from/to JSON objects
 */
class PythonEngineJSONNRPClient
        : public EngineJSONNRPClient<PythonEngineJSONNRPClient, PythonConfig, PyObjectDevice>
{
		/*!
		 * \brief Time (in seconds) to wait for Python to exit cleanly after first SIGTERM signal. Afterwards, send a SIGKILL
		 */
		static constexpr size_t _killWait = 10;

	public:
		PythonEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~PythonEngineJSONNRPClient() override;

		virtual RESULT initialize() override;

		virtual RESULT shutdown() override;

	private:
		/*!
		 * \brief Error message returned by init command
		 */
		std::string _initErrMsg = "";
};

using PythonEngineJSONLauncher = PythonEngineJSONNRPClient::EngineLauncher<PythonConfig::DefEngineType>;


CREATE_NRP_ENGINE_LAUNCHER(PythonEngineJSONLauncher);


#endif // PYTHON_ENGINE_JSON_NRP_CLIENT_H
