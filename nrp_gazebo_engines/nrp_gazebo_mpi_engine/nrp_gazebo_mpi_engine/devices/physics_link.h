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

#ifndef PHYSICS_LINK_H
#define PHYSICS_LINK_H

#include "nrp_general_library/device_interface/device.h"

class PhysicsLink;

struct PhysicsLinkConst
{
		using vec3_t = std::array<float, 3>;
		using quat_t = std::array<float, 4>;

		static constexpr FixedString Position = "pos";
		static constexpr vec3_t DefPosition = {0,0,0};

		static constexpr FixedString Rotation = "rot";
		static constexpr quat_t DefRotation = {0,0,0,0};

		static constexpr FixedString LinearVelocity = "lin_vel";
		static constexpr vec3_t DefLinearVelocity = {0,0,0};

		static constexpr FixedString AngularVelocity = "ang_vel";
		static constexpr vec3_t DefAngularVelocity = {0,0,0};

		static constexpr std::string_view TypeName = "p_link";

		using JPropNames = PropNames<Position, Rotation, LinearVelocity, AngularVelocity>;
		using JProps = PropertyTemplate<PhysicsLink, PhysicsLinkConst::JPropNames,
		                                std::array<float, 3>, std::array<float, 4>, std::array<float, 3>, std::array<float, 3> >;
};

class PhysicsLink
        : public PhysicsLinkConst,
          public DeviceInterface,
          public PhysicsLinkConst::JProps
{
	public:
		PhysicsLink() = default;
		PhysicsLink(const std::string &name);
		PhysicsLink(const DeviceIdentifier &id);

		const vec3_t &position() const;
		void setPosition(const vec3_t &position);

		const quat_t &rotation() const;
		void setRotation(const quat_t &rotation);

		const vec3_t &linVel() const;
		void setLinVel(const vec3_t &linVel);

		const vec3_t &angVel() const;
		void setAngVel(const vec3_t &angVel);
};

#endif // JSON_PHYSICS_LINK_H
