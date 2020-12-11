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

#ifndef LINK_DEVICE_CONTROLLER_H
#define LINK_DEVICE_CONTROLLER_H

#include "nrp_gazebo_devices/physics_link.h"
#include "nrp_general_library/engine_interfaces/engine_device_controller.h"

#include <gazebo/gazebo.hh>
#include <gazebo/physics/Link.hh>

namespace gazebo
{
	/*!
	 * \brief Interface for links
	 */
	template<class SERIALIZATION>
	class LinkDeviceController
	        : public EngineDeviceController<SERIALIZATION, PhysicsLink>
	{
			template<class T>
			constexpr static float ToFloat(const T &val)
			{	return static_cast<float>(val);	}

		public:
			LinkDeviceController(const std::string &linkName, const physics::LinkPtr &link)
			    : EngineDeviceController<SERIALIZATION, PhysicsLink>(PhysicsLink::createID(linkName, "")),
			      _data(DeviceIdentifier(*this)),
			      _link(link)
			{}

			virtual void handleDeviceDataCallback(PhysicsLink &&data) override
			{}

			virtual const PhysicsLink *getDeviceInformationCallback() override
			{
				const auto &pose = this->_link->WorldCoGPose();
				this->_data.setPosition({ ToFloat(pose.Pos().X()), ToFloat(pose.Pos().Y()), ToFloat(pose.Pos().Z())	});
				this->_data.setRotation({ ToFloat(pose.Rot().X()), ToFloat(pose.Rot().Y()), ToFloat(pose.Rot().Z())	});

				const auto &linVel = this->_link->WorldLinearVel();
				this->_data.setLinVel({ ToFloat(linVel.X()), ToFloat(linVel.Y()), ToFloat(linVel.Z())	});

				const auto &angVel = this->_link->WorldAngularVel();
				this->_data.setAngVel({ ToFloat(angVel.X()), ToFloat(angVel.Y()), ToFloat(angVel.Z())	});

				return &(this->_data);
			}

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
}

#endif // LINK_DEVICE_CONTROLLER_H
