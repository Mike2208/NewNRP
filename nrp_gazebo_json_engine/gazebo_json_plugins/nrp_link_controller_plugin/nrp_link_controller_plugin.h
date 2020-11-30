#ifndef NRP_LINK_CONTROLLER_PLUGIN_H
#define NRP_LINK_CONTROLLER_PLUGIN_H

#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"
#include "nrp_gazebo_json_engine/devices/physics_link.h"

#include <gazebo/gazebo.hh>

namespace gazebo
{
	/*!
	 * \brief Interface for links
	 */
	class LinkDeviceController
	        : public EngineJSONDeviceController<PhysicsLink>
	{
		public:
			LinkDeviceController(const std::string &linkName, const physics::LinkPtr &link);

			virtual void handleDeviceDataCallback(PhysicsLink &&data) override;
			virtual const PhysicsLink *getDeviceInformationCallback() override;

		private:
			/*!
			 * \brief Link Data
			 */
			PhysicsLink _data;

			/*!
			 * \brief Pointer to link
			 */
			physics::LinkPtr _link;
	};

	class NRPLinkControllerPlugin
	        : public gazebo::ModelPlugin
	{
		public:
			virtual void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

		private:
			std::list<LinkDeviceController> _linkInterfaces;
	};

	GZ_REGISTER_MODEL_PLUGIN(NRPLinkControllerPlugin)
}

#endif
