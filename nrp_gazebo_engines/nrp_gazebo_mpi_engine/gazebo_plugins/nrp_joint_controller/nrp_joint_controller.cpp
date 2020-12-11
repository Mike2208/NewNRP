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

#include "nrp_joint_controller/nrp_joint_controller.h"

#include "nrp_communication_controller/nrp_communication_controller.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <algorithm>
#include <exception>

#include <gazebo/physics/Joint.hh>
#include <gazebo/physics/JointController.hh>
#include <gazebo/physics/Model.hh>
#include <gazebo/physics/World.hh>


using namespace nlohmann;

gazebo::JointDeviceController::JointDeviceController(const physics::JointPtr &joint, const gazebo::physics::JointControllerPtr &jointController, const std::string &jointName)
    : EngineMPIDeviceController(DeviceIdentifier(jointName, "", PhysicsJoint::TypeName.data())),
      _joint(joint),
      _jointController(jointController),
      _jointData(static_cast<const DeviceIdentifier&>(*this))
{}

MPIPropertyData gazebo::JointDeviceController::getDeviceOutput()
{
	this->_jointData.setPosition(this->_joint->Position(0));
	this->_jointData.setVelocity(this->_joint->GetVelocity(0));
	this->_jointData.setEffort(this->_joint->GetForce(0));

	return MPIPropertySerializer<PhysicsJoint>::serializeProperties(this->_jointData);
}

void gazebo::JointDeviceController::handleDeviceInput(PhysicsJoint &data)
{
	bool success = 1;

	this->_jointData = data;

	const auto &jointName = this->_jointData.name();
	if(!std::isnan(this->_jointData.position()))
		success &= this->_jointController->SetPositionTarget(jointName, this->_jointData.position());

	if(!std::isnan(this->_jointData.velocity()))
		success &= this->_jointController->SetVelocityTarget(jointName, this->_jointData.velocity());

	if(!std::isnan(this->_jointData.effort()))
		this->_joint->SetForce(0, this->_jointData.effort());

	if(!success)
		throw NRPException::logCreate("Handling received joint devices for \"" + this->EngineName +  "\" failed");
}

gazebo::NRPJointController::PIDConfig::PIDConfig(PID _pid, gazebo::NRPJointController::PIDConfig::PID_TYPE _type)
    : gazebo::common::PID(_pid), Type(_type)
{}

gazebo::NRPJointController::PIDConfig::PID_TYPE gazebo::NRPJointController::PIDConfig::convertStringToType(std::string type)
{
	std::transform(type.begin(), type.end(), type.begin(), ::tolower);

	if(type.compare("position") == 0)
		return POSITION;

	if(type.compare("velocity") == 0)
		return VELOCITY;

	throw NRPException::logCreate("No PID of type " + type + " known");
}

gazebo::NRPJointController::~NRPJointController() = default;

void gazebo::NRPJointController::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf)
{
	std::map<std::string, PIDConfig> jointConfigs;

	// Iterate over sdf configurations
	try
	{
		// Read configuration for all joints
		auto pJointPID = sdf->GetFirstElement();
		while(pJointPID != nullptr)
		{
			const auto jointName = pJointPID->GetName();

			// Find corresponding joint data
			const auto pJoint = model->GetJoint(jointName);
			if(pJoint == nullptr)
				throw NRPException::logCreate("Joint \"" + jointName + "\" not found in model \"" + model->GetScopedName() + "\"");

			// Read PID settings
			PIDConfig jointConfig(common::PID(pJointPID->Get<double>("P"), pJointPID->Get<double>("I"), pJointPID->Get<double>("D")),
			                      PIDConfig::convertStringToType(pJointPID->Get<std::string>("Type")));

			// Set target
			const auto defTarget = jointConfig.Type == PIDConfig::POSITION ? pJoint->Position(0) : pJoint->GetVelocity(0);
			jointConfig.SetCmd(NRPJointController::getOptionalValue<double>(pJointPID, "Target", defTarget));

			// Set I max and min vals
			jointConfig.SetIMax(NRPJointController::getOptionalValue<double>(pJointPID, "IMax", 0));
			jointConfig.SetIMin(NRPJointController::getOptionalValue<double>(pJointPID, "IMin", 0));

			// Save config for later
			jointConfigs.emplace(jointName, jointConfig);

			pJointPID = pJointPID->GetNextElement();
		}
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Error reading configuration for plugin \"" + this->GetHandle() + "\" of model \"" + model->GetScopedName() + "\"");
	}

	// Initiate PID controllers with read values
	auto jointControllerPtr = model->GetJointController();

	// Initiate device interfaces for joints
	const auto &joints = model->GetJoints();
	for(const auto &joint : joints)
	{
		const auto jointName = joint->GetScopedName();

		// Check for existing PID Config
		auto pidConfigIterator = jointConfigs.find(joint->GetName());
		if(pidConfigIterator != jointConfigs.end())
		{
			// Apply configuration
			const auto &pidConfig = pidConfigIterator->second;
			if(pidConfig.Type == PIDConfig::PID_TYPE::POSITION)
				jointControllerPtr->SetPositionPID(jointName, pidConfig);
			else if(pidConfig.Type == PIDConfig::PID_TYPE::VELOCITY)
				jointControllerPtr->SetVelocityPID(jointName, pidConfig);
			else
			{	/* TODO: Handle invalid controller type (Should already have been done with PIDConfig::convertStringToType, but maybe make sure here as well?) */}
		}

		// Create device
		const auto deviceName = NRPCommunicationController::createDeviceName(*this, joint->GetName());

		std::cout << "Registering joint controller for joint \"" << jointName << "\"\n";
		this->_jointDeviceControllers.push_back(JointDeviceController(joint, jointControllerPtr, jointName));
		NRPCommunicationController::getInstance().registerDeviceController(&(this->_jointDeviceControllers.back()));
	}
}
