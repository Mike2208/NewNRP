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
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

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

		using JPropNames = PropNames<Position, Rotation, LinearVelocity, AngularVelocity>;
};

class PhysicsLink
        : public PhysicsLinkConst,
          public Device<PhysicsLink, "PhysicsLink", PhysicsLinkConst::JPropNames, std::array<float, 3>, std::array<float, 4>, std::array<float, 3>, std::array<float, 3> >
{
	public:
		PhysicsLink(DeviceIdentifier &&devID, property_template_t &&props = property_template_t(DefPosition, DefRotation, DefLinearVelocity, DefAngularVelocity))
		    : Device(std::move(devID), std::move(props))
		{}

		template<class DESERIALIZE_T>
		static auto deserializeProperties(DESERIALIZE_T &&data)
		{	return Device::deserializeProperties(std::forward<DESERIALIZE_T>(data), DefPosition, DefRotation, DefLinearVelocity, DefAngularVelocity);	}

		const vec3_t &position() const;
		void setPosition(const vec3_t &position);

		const quat_t &rotation() const;
		void setRotation(const quat_t &rotation);

		const vec3_t &linVel() const;
		void setLinVel(const vec3_t &linVel);

		const vec3_t &angVel() const;
		void setAngVel(const vec3_t &angVel);
};

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PhysicsLinkConst::vec3_t &property);

template<>
PhysicsLinkConst::vec3_t JSONPropertySerializerMethods::deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name);

/*! \addtogroup gazebo_devices
 * The PhysicsJoint Device consists of the following attributes:
 * <table>
 * <caption id="physics_joint_attributes_table">Physics Joint Attributes</caption>
 * <tr><th>Attribute  <th>Description                                 <th>Python Type                   <th>C type
 * <tr><td>pos        <td>Joint Position                              <td>numpy.array(3, numpy.float32) <td>std::array<float,3>
 * <tr><td>rot        <td>Joint Rotation as quaternion                <td>numpy.array(4, numpy.float32) <td>std::array<float,4>
 * <tr><td>lin_vel    <td>Joint Linear Velocity                       <td>numpy.array(3, numpy.float32) <td>std::array<float,3>
 * <tr><td>ang_vel    <td>Joint Angular Velocity                      <td>numpy.array(3, numpy.float32) <td>std::array<float,3>
 * </table>
 */


#endif // JSON_PHYSICS_LINK_H
