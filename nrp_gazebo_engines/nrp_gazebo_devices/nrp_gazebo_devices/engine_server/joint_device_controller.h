#ifndef JOINT_DEVICE_CONTROLLER_H
#define JOINT_DEVICE_CONTROLLER_H

#include "nrp_gazebo_devices/physics_joint.h"
#include "nrp_general_library/engine_interfaces/engine_device_controller.h"

#include <gazebo/gazebo.hh>
#include <gazebo/physics/JointController.hh>
#include <gazebo/physics/Joint.hh>

namespace gazebo
{
	/*!
	 * \brief Interface for a single joint
	 */
	template<class SERIALIZATION>
	class JointDeviceController
	        : public EngineDeviceController<SERIALIZATION, PhysicsJoint>
	{
			using fcn_ptr_t = void(physics::JointPtr, double, int);

		public:
			JointDeviceController(const physics::JointPtr &joint, const physics::JointControllerPtr &jointController, const std::string &jointName)
			    : EngineDeviceController<SERIALIZATION, PhysicsJoint>(PhysicsJoint::createID(jointName, "")),
			      _joint(joint),
			      _jointController(jointController),
			      _jointData(DeviceIdentifier(*this))
			{}

			virtual void handleDeviceDataCallback(PhysicsJoint &&data) override
			{
				this->_jointData = std::move(data);

				//	std::cout << std::to_string(this->_jointData.position()) << std::endl;
				//	std::cout << std::to_string(this->_jointData.velocity()) << std::endl;
				//	std::cout << std::to_string(this->_jointData.effort()) << std::endl;

				const auto &jointName = this->_jointData.name();
				if(!std::isnan(this->_jointData.position()))
					this->_jointController->SetPositionTarget(jointName, this->_jointData.position());

				if(!std::isnan(this->_jointData.velocity()))
					this->_jointController->SetVelocityTarget(jointName, this->_jointData.velocity());

				if(!std::isnan(this->_jointData.effort()))
					this->_joint->SetForce(0, this->_jointData.effort());
			}

			virtual const PhysicsJoint *getDeviceInformationCallback() override
			{
				this->_jointData.setPosition(this->_joint->Position(0));
				this->_jointData.setVelocity(this->_joint->GetVelocity(0));
				this->_jointData.setEffort(this->_joint->GetForce(0));

				return &(this->_jointData);
			}

		private:
			/*!
			 * \brief Pointer to joint
			 */
			physics::JointPtr _joint;

			/*!
			 * \brief Pointer to joint controller of the joint's model
			 */
			physics::JointControllerPtr _jointController = nullptr;

			/*!
			 * \brief Data of link
			 */
			PhysicsJoint _jointData;
	};
}

#endif // JOINT_DEVICE_CONTROLLER_H
