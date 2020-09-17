#ifndef GAZEBO_ENGINE_JSON_NRP_CLIENT_H
#define GAZEBO_ENGINE_JSON_NRP_CLIENT_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/nrp_client/engine_json_nrp_client.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/plugin_system/plugin.h"

#include "nrp_gazebo_device_interface/devices/physics_camera.h"
#include "nrp_gazebo_device_interface/devices/physics_joint.h"
#include "nrp_gazebo_device_interface/devices/physics_link.h"

#include "nrp_gazebo_device_interface/config/gazebo_config.h"

#include <unistd.h>

/*! \defgroup gazebo_json_engine "Gazebo REST JSON Engine"
Gazebo Engine based on the \ref json_engine.
 */

/*! \addtogroup gazebo_json_engine
 *  \brief NRP - Gazebo Communicator on the NRP side. Converts DeviceInterface classes from/to JSON objects
 */
class GazeboEngineJSONNRPClient
        : public EngineJSONNRPClient<GazeboEngineJSONNRPClient, GazeboConfig, PhysicsCamera, PhysicsJoint, PhysicsLink>
{
	public:
		static constexpr FixedString DefEngineName = "gazebo_json";

		GazeboEngineJSONNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~GazeboEngineJSONNRPClient() override = default;

		virtual RESULT initialize() override;

		virtual RESULT shutdown() override;
};

using GazeboEngineJSONLauncher = GazeboEngineJSONNRPClient::EngineLauncher<GazeboEngineJSONNRPClient::DefEngineName>;

CREATE_NRP_ENGINE_LAUNCHER(GazeboEngineJSONLauncher);

#endif // GAZEBO_ENGINE_JSON_NRP_CLIENT_H
