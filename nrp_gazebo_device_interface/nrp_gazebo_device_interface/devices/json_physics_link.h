#ifndef JSON_PHYSICS_LINK_H
#define JSON_PHYSICS_LINK_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_interface.h"

struct JSONPhysicsLinkConst
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
};

class JSONPhysicsLink
        : public JSONDeviceInterface<JSONPhysicsLink, JSONPhysicsLinkConst::JPropNames,
                                     std::array<float, 3>, std::array<float, 4>, std::array<float, 3>, std::array<float, 3> >,
          public JSONPhysicsLinkConst
{
	public:
		JSONPhysicsLink(const std::string &name);
		JSONPhysicsLink(const DeviceIdentifier &id);
		JSONPhysicsLink(const DeviceIdentifier &id, const nlohmann::json &data);

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
