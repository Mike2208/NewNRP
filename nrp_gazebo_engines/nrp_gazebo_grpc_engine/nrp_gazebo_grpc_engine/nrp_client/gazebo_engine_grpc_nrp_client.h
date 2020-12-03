#ifndef GAZEBO_ENGINE_GRPC_NRP_CLIENT_H
#define GAZEBO_ENGINE_GRPC_NRP_CLIENT_H

#include "nrp_grpc_engine_protocol/engine_client/engine_grpc_client.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/plugin_system/plugin.h"

#include "nrp_gazebo_grpc_engine/devices/grpc_physics_camera.h"
#include "nrp_gazebo_grpc_engine/devices/grpc_physics_joint.h"
#include "nrp_gazebo_grpc_engine/devices/grpc_physics_link.h"

#include "nrp_gazebo_grpc_engine/config/gazebo_config.h"

#include <unistd.h>

/*! \defgroup gazebo_json_engine "Gazebo REST JSON Engine"
Gazebo Engine based on the \ref json_engine. The client is defined in GazeboEngineJSONNRPClient.
 */

/*!
 *  \brief NRP - Gazebo Communicator on the NRP side. Converts DeviceInterface classes from/to JSON objects
 */
class GazeboEngineGrpcNRPClient
        : public EngineGrpcClient<GazeboEngineGrpcNRPClient, GazeboConfig, PhysicsCamera, PhysicsJoint, PhysicsLink>
{
	public:
		GazeboEngineGrpcNRPClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~GazeboEngineGrpcNRPClient() override = default;

		virtual void initialize() override;

		virtual void shutdown() override;
};

using GazeboEngineGrpcLauncher = GazeboEngineGrpcNRPClient::EngineLauncher<GazeboConfig::DefEngineType>;

CREATE_NRP_ENGINE_LAUNCHER(GazeboEngineGrpcLauncher);

#endif // GAZEBO_ENGINE_GRPC_NRP_CLIENT_H
