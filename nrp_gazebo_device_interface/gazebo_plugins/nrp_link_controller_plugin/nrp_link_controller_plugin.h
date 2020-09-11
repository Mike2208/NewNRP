#ifndef NRP_LINK_CONTROLLER_PLUGIN_H
#define NRP_LINK_CONTROLLER_PLUGIN_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"
#include "nrp_gazebo_device_interface/devices/physics_link.h"

#include <gazebo/gazebo.hh>

namespace gazebo
{
	/*!
	 * \brief Interface for links
	 */
	class LinkDeviceController
	        : public EngineJSONDeviceController
	{
		public:
			LinkDeviceController(const std::string &linkName, const physics::LinkPtr &link);
			virtual ~LinkDeviceController() override;

			/*!
			 * \brief Gets link device information, namely link pose and velocity
			 * \param data Not used
			 * \return Returns link information as JSON
			 */
			virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator &data) override;
			virtual nlohmann::json handleDeviceData(const nlohmann::json &data) override;

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
			virtual ~NRPLinkControllerPlugin();

			virtual void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

		private:

			std::list<LinkDeviceController> _linkInterfaces;
	};

	GZ_REGISTER_MODEL_PLUGIN(NRPLinkControllerPlugin)
}

#endif
