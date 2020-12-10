/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef NRP_JOINT_CONTROLLER_H
#define NRP_JOINT_CONTROLLER_H

#include "nrp_gazebo_devices/engine_server/joint_device_controller.h"
#include "nrp_gazebo_grpc_engine/devices/grpc_physics_joint.h"
#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"


namespace gazebo
{
	class NRPJointController
	        : public gazebo::ModelPlugin
	{
			struct PIDConfig
			        : public common::PID
			{
				enum PID_TYPE { VELOCITY, POSITION };

				PID_TYPE Type = POSITION;

				PIDConfig(PID &&_pid, PID_TYPE _type);

				static PID_TYPE convertStringToType(std::string type);
			};

		public:
			virtual ~NRPJointController() override;

			virtual void Load(physics::ModelPtr model, sdf::ElementPtr sdf) override;

		private:

			/*!
			 * \brief List containing all joint interfaces. TODO: Change to shared_ptr to prevent segfault errors when this plugin is destroyed
			 */
			std::list<GrpcDeviceControlSerializer<JointDeviceController> > _jointDeviceControllers;

			/*!
			 * \brief Joint PID Configuration
			 */
			std::map<std::string, PIDConfig> _jointConfigs;

			template<class T>
			static T getOptionalValue(const sdf::ElementPtr &pidConfig, const std::string &key, T defaultValue);
	};

	GZ_REGISTER_MODEL_PLUGIN(NRPJointController)

	template<class T>
	T NRPJointController::getOptionalValue(const sdf::ElementPtr &pidConfig, const std::string &key, T defaultValue)
	{
		try
		{
			return pidConfig->Get<T>(key);
		}
		catch(const std::exception &e)
		{
			std::cerr << e.what();
		}

		return defaultValue;
	}
}

#endif
