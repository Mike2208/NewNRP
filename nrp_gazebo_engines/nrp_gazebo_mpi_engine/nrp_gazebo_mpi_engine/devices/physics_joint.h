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

#ifndef PHYSICS_JOINT_H
#define PHYSICS_JOINT_H

#include "nrp_general_library/device_interface/device.h"

class PhysicsJoint;

struct PhysicsJointConst
{
	struct FloatNan
	{
			FloatNan() = default;
			FloatNan(float val);

			inline operator float() const
			{	return this->_val;	}

			inline operator float&()
			{	return this->_val;	}

		private:
			float _val;
	};

	static constexpr FixedString Position = "pos";
	static constexpr FixedString Velocity = "vel";
	static constexpr FixedString Effort = "eff";

	static constexpr std::string_view TypeName = "p_joint";

	using JPropNames = PropNames<Position, Velocity, Effort>;
	using JProps = PropertyTemplate<PhysicsJoint, PhysicsJointConst::JPropNames, PhysicsJointConst::FloatNan, PhysicsJointConst::FloatNan, PhysicsJointConst::FloatNan>;
};

/*!
 * \brief Joint Device
 */
class PhysicsJoint
        : public PhysicsJointConst,
          public DeviceInterface,
          public PhysicsJointConst::JProps
{
	public:
		PhysicsJoint(const std::string &name);
		PhysicsJoint(const DeviceIdentifier &id);
		PhysicsJoint() = default;

		float position() const;
		void setPosition(float position);

		float velocity() const;
		void setVelocity(float velocity);

		float effort() const;
		void setEffort(float effort);
};

#endif // PHYSICS_JOINT_H
