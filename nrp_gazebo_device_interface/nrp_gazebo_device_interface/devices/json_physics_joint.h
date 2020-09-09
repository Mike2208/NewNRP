#ifndef JSON_PHYSICS_JOINT_H
#define JSON_PHYSICS_JOINT_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_interface.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

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
};

/*!
 * \brief Joint Device
 */
class PhysicsJoint
        : public PhysicsJointConst,
          public JSONDeviceInterface<PhysicsJoint, PhysicsJointConst::JPropNames, PhysicsJointConst::FloatNan, PhysicsJointConst::FloatNan, PhysicsJointConst::FloatNan>
{
	public:
		PhysicsJoint(const std::string &name);
		PhysicsJoint(const DeviceIdentifier &id);
		PhysicsJoint(const DeviceIdentifier &id, const nlohmann::json &data);

		float position() const;
		void setPosition(float position);

		float velocity() const;
		void setVelocity(float velocity);

		float effort() const;
		void setEffort(float effort);
};

template<>
nlohmann::json ObjectPropertySerializerMethods<nlohmann::json>::serializeSingleProperty(const PhysicsJointConst::FloatNan &property);

template<>
PhysicsJointConst::FloatNan ObjectPropertySerializerMethods<nlohmann::json>::deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name);


#endif // JSON_PHYSICS_JOINT_H
