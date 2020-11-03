#include "nrp_link_controller_plugin/nrp_link_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

#include <gazebo/physics/Model.hh>
#include <gazebo/physics/Link.hh>

using namespace nlohmann;

template<class T>
inline float ToFloat(const T &val)
{
	return static_cast<float>(val);
}

gazebo::LinkDeviceController::LinkDeviceController(const std::string &linkName, const gazebo::physics::LinkPtr &link)
    : EngineGrpcDeviceController(DeviceIdentifier(linkName, PhysicsLink::TypeName.data(), "")),
      _data(linkName),
      _link(link)
{}

gazebo::LinkDeviceController::~LinkDeviceController() = default;

const google::protobuf::Message *  gazebo::LinkDeviceController::getData()
{
	EngineGrpc::GazeboLink * linkData = new EngineGrpc::GazeboLink();

	const auto &pose = this->_link->WorldCoGPose();

	linkData->add_position(ToFloat(pose.Pos().X()));
	linkData->add_position(ToFloat(pose.Pos().Y()));
	linkData->add_position(ToFloat(pose.Pos().Z()));

	linkData->add_rotation(ToFloat(pose.Rot().X()));
	linkData->add_rotation(ToFloat(pose.Rot().Y()));
	linkData->add_rotation(ToFloat(pose.Rot().Z()));

	const auto &linVel = this->_link->WorldLinearVel();

	linkData->add_linearvelocity(ToFloat(linVel.X()));
	linkData->add_linearvelocity(ToFloat(linVel.Y()));
	linkData->add_linearvelocity(ToFloat(linVel.Z()));

	const auto &angVel = this->_link->WorldAngularVel();

	linkData->add_angularvelocity(ToFloat(angVel.X()));
	linkData->add_angularvelocity(ToFloat(angVel.Y()));
	linkData->add_angularvelocity(ToFloat(angVel.Z()));

	return linkData;
}

void gazebo::LinkDeviceController::setData(const google::protobuf::Message &)
{
	// Do nothing
}

gazebo::NRPLinkControllerPlugin::~NRPLinkControllerPlugin() = default;

void gazebo::NRPLinkControllerPlugin::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr)
{
	auto &commControl = NRPCommunicationController::getInstance();

	// Register a device for each link
	auto links = model->GetLinks();
	for(const auto &link : links)
	{
		const auto deviceName = NRPCommunicationController::createDeviceName(*this, link->GetName());

		std::cout << "Registering link controller for link \"" << deviceName << "\"\n";

		this->_linkInterfaces.push_back(LinkDeviceController(deviceName, link));
		commControl.registerDevice(deviceName, &(this->_linkInterfaces.back()));
	}
}
