#ifndef NEST_ENGINE_JSON_NRP_CLIENT_H
#define NEST_ENGINE_JSON_NRP_CLIENT_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/nrp_client/engine_json_nrp_client.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/plugin_system/plugin.h"

#include "nrp_nest_device_interface/devices/nest_json_device_interface.h"

#include "nrp_nest_device_interface/config/nest_config.h"

#include <unistd.h>

/*!
 * \brief NRP - Nest Communicator on the NRP side. Converts DeviceInterface classes from/to JSON objects
 */
class NestEngineJSONNRPClient
        : public EngineJSONNRPClient<NestEngineJSONNRPClient, NestConfig, NestJSONDeviceInterface>
{
		/*!
		 * \brief Number of seconds to wait for Nest to exit cleanly after first SIGTERM signal. Afterwards, send a SIGKILL
		 */
		static constexpr size_t _killWait = 10;

	public:
		static constexpr FixedString DefEngineName = "nest_json";

		NestEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~NestEngineJSONNRPClient() override;

		virtual RESULT initialize() override;

		virtual RESULT shutdown() override;

	private:
		/*!
		 * \brief Error message returned by init command
		 */
		std::string _initErrMsg = "";
};

using NestEngineJSONLauncher = NestEngineJSONNRPClient::EngineLauncher<NestEngineJSONNRPClient::DefEngineName>;


CREATE_NRP_ENGINE_LAUNCHER(NestEngineJSONLauncher);


#endif // NEST_ENGINE_JSON_NRP_CLIENT_H
