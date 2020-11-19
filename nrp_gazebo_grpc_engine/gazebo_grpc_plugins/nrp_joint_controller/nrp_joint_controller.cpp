#include "nrp_joint_controller/nrp_joint_controller.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

#include <algorithm>
#include <exception>

#include <gazebo/physics/Joint.hh>
#include <gazebo/physics/JointController.hh>
#include <gazebo/physics/Model.hh>
#include <gazebo/physics/World.hh>


using namespace nlohmann;

gazebo::JointDeviceController::JointDeviceController(const physics::JointPtr &joint, const gazebo::physics::JointControllerPtr &jointController, const std::string &jointName)
    : EngineGrpcDeviceController(DeviceIdentifier(jointName, PhysicsJoint::TypeName.data(), "")),
      _joint(joint),
      _jointController(jointController),
      _jointData(DeviceIdentifier(jointName, PhysicsJoint::TypeName.data(), ""))
{}

bool gazebo::JointDeviceController::getData(EngineGrpc::GetDeviceMessage * reply)
{
	reply->mutable_joint()->set_position(this->_joint->Position(0));
	reply->mutable_joint()->set_velocity(this->_joint->GetVelocity(0));
	reply->mutable_joint()->set_effort(this->_joint->GetForce(0));

	return true;
}

void gazebo::JointDeviceController::setData(const google::protobuf::Message & data)
{
	// TODO Throw an exception on error?
	bool success = true;

	auto jointData = static_cast<const EngineGrpc::GazeboJointSetData &>(data);

	// TODO Is this passed with data?
	const auto &jointName = this->_jointData.name();

	_jointData.setPosition(jointData.position());
	_jointData.setVelocity(jointData.velocity());
	_jointData.setEffort(jointData.effort());

	if(!std::isnan(this->_jointData.position()))
		success &= this->_jointController->SetPositionTarget(jointName, this->_jointData.position());

	if(!std::isnan(this->_jointData.velocity()))
		success &= this->_jointController->SetVelocityTarget(jointName, this->_jointData.velocity());

	if(!std::isnan(this->_jointData.effort()))
		this->_joint->SetForce(0, this->_jointData.effort());
}

gazebo::NRPJointController::PIDConfig::PIDConfig(PID &&_pid, gazebo::NRPJointController::PIDConfig::PID_TYPE _type)
    : gazebo::common::PID(std::move(_pid)), Type(_type)
{}

gazebo::NRPJointController::PIDConfig::PID_TYPE gazebo::NRPJointController::PIDConfig::convertStringToType(std::string type)
{
	std::transform(type.begin(), type.end(), type.begin(), ::tolower);

	if(type.compare("position") == 0)
		return POSITION;

	if(type.compare("velocity") == 0)
		return VELOCITY;

	throw std::invalid_argument("No PID of type " + type + " known");
}

gazebo::NRPJointController::~NRPJointController() = default;

void gazebo::NRPJointController::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf)
{
	std::map<std::string, double> jointTargets;

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
			{
				const auto errMsg = "Joint \"" + jointName + "\" not found in model \"" + model->GetScopedName() + "\"";
				std::cerr << errMsg << std::endl;
				throw std::logic_error(errMsg);
			}

			// Read PID settings
			PIDConfig jointConfig(common::PID(pJointPID->Get<double>("P"), pJointPID->Get<double>("I"), pJointPID->Get<double>("D")),
			                      PIDConfig::convertStringToType(pJointPID->Get<std::string>("Type")));

			// Save target
			const auto defTarget = jointConfig.Type == PIDConfig::POSITION ? pJoint->Position(0) : pJoint->GetVelocity(0);
			jointTargets.emplace(jointName, NRPJointController::getOptionalValue<double>(pJointPID, "Target", defTarget));

			// Set I max and min vals
			jointConfig.SetIMax(NRPJointController::getOptionalValue<double>(pJointPID, "IMax", 0));
			jointConfig.SetIMin(NRPJointController::getOptionalValue<double>(pJointPID, "IMin", 0));

			// Save config for later
			this->_jointConfigs.emplace(jointName, jointConfig);

			pJointPID = pJointPID->GetNextElement();
		}
	}
	catch(const std::exception &)
	{
		std::cerr << "Error reading configuration for plugin \"" << this->GetHandle() << "\" of model \"" << model->GetScopedName() << "\"" << std::endl;
		throw;
	}

	// Initiate PID controllers with read values
	auto jointControllerPtr = model->GetJointController();

	// Initiate device interfaces for joints
	const auto &joints = model->GetJoints();
	for(const auto &joint : joints)
	{
		const auto jointName = joint->GetScopedName();

		// Check for existing PID Config
		auto pidConfigIterator = this->_jointConfigs.find(joint->GetName());
		if(pidConfigIterator != this->_jointConfigs.end())
		{
			// Apply configuration
			const auto &pidConfig = pidConfigIterator->second;
			if(pidConfig.Type == PIDConfig::PID_TYPE::POSITION)
			{
				jointControllerPtr->SetPositionPID(jointName, pidConfig);
				jointControllerPtr->SetPositionTarget(jointName, jointTargets.at(joint->GetName()));
			}
			else if(pidConfig.Type == PIDConfig::PID_TYPE::VELOCITY)
			{
				jointControllerPtr->SetVelocityPID(jointName, pidConfig);
				jointControllerPtr->SetVelocityTarget(jointName, jointTargets.at(joint->GetName()));
			}
			else
			{	/* TODO: Handle invalid controller type (Should already have been done with PIDConfig::convertStringToType, but maybe make sure here as well?) */}
		}

		// Create device
		const auto deviceName = NRPCommunicationController::createDeviceName(*this, joint->GetName());

		std::cout << "Registering joint controller for joint \"" << jointName << "\"\n";
		this->_jointDeviceControllers.push_back(JointDeviceController(joint, jointControllerPtr, jointName));
		NRPCommunicationController::getInstance().registerDevice(deviceName, &(this->_jointDeviceControllers.back()));
	}
}
