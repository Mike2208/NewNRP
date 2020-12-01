#include "nrp_link_controller_plugin/nrp_link_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

#include <gazebo/physics/Model.hh>
#include <gazebo/physics/Link.hh>

using namespace nlohmann;

void gazebo::NRPLinkControllerPlugin::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr)
{
	auto &commControl = NRPCommunicationController::getInstance();

	// Register a device for each link
	auto links = model->GetLinks();
	for(const auto &link : links)
	{
		const auto deviceName = NRPCommunicationController::createDeviceName(*this, link->GetName());

		std::cout << "Registering link controller for link \"" << deviceName << "\"\n";

		this->_linkInterfaces.push_back(GrpcDeviceControlSerializer<LinkDeviceController>(deviceName, link));
		commControl.registerDevice(deviceName, &(this->_linkInterfaces.back()));
	}
}
