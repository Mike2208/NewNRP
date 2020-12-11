//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_link_controller_plugin/nrp_link_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

#include <gazebo/physics/Model.hh>
#include <gazebo/physics/Link.hh>

// Convert val to float. Only used to prevent compiler warnings when converting an Eigen array to PhysicsLink
template<class T>
inline float ToFloat(const T &val)
{
	return static_cast<float>(val);
}

gazebo::LinkDeviceController::LinkDeviceController(const std::string &linkName, const gazebo::physics::LinkPtr &link)
    : EngineMPIDeviceController(DeviceIdentifier(linkName, "", PhysicsLink::TypeName.data())),
      _data(linkName),
      _link(link)
{}

gazebo::LinkDeviceController::~LinkDeviceController() = default;

MPIPropertyData gazebo::LinkDeviceController::getDeviceOutput()
{
	const auto &pose = this->_link->WorldCoGPose();
	this->_data.setPosition({ ToFloat(pose.Pos().X()), ToFloat(pose.Pos().Y()), ToFloat(pose.Pos().Z())	});
	this->_data.setRotation({ ToFloat(pose.Rot().X()), ToFloat(pose.Rot().Y()), ToFloat(pose.Rot().Z())	});

	const auto &linVel = this->_link->WorldLinearVel();
	this->_data.setLinVel({ ToFloat(linVel.X()), ToFloat(linVel.Y()), ToFloat(linVel.Z())	});

	const auto &angVel = this->_link->WorldAngularVel();
	this->_data.setAngVel({ ToFloat(angVel.X()), ToFloat(angVel.Y()), ToFloat(angVel.Z())	});

	return MPIPropertySerializer<PhysicsLink>::serializeProperties(this->_data);
}

void gazebo::LinkDeviceController::handleDeviceInput(PhysicsLink &data)
{}

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
		commControl.registerDeviceController(&(this->_linkInterfaces.back()));
	}
}
