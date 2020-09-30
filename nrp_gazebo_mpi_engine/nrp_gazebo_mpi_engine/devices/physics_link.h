#ifndef PHYSICS_LINK_H
#define PHYSICS_LINK_H

#include "nrp_general_library/device_interface/device_interface.h"

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
		using json_property_serializer_t = JSONPropertySerializer<JProps>;

	public:
		PhysicsLink(const std::string &name);
		PhysicsLink(const DeviceIdentifier &id);
		PhysicsLink(const DeviceIdentifier &id, const nlohmann::json &data);

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
