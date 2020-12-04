#ifndef NRP_LINK_CONTROLLER_PLUGIN_H
#define NRP_LINK_CONTROLLER_PLUGIN_H

#include "nrp_gazebo_devices/engine_server/link_device_controller.h"
#include "nrp_gazebo_grpc_engine/devices/grpc_physics_link.h"
#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"

#include <gazebo/gazebo.hh>

namespace gazebo
{
	class NRPLinkControllerPlugin
	        : public gazebo::ModelPlugin
	{
		public:
			virtual void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

		private:

			std::list<GrpcDeviceControlSerializer<LinkDeviceController> > _linkInterfaces;
	};

	GZ_REGISTER_MODEL_PLUGIN(NRPLinkControllerPlugin)
}

#endif
