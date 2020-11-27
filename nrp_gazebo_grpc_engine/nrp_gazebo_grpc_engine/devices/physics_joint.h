#ifndef PHYSICS_JOINT_H
#define PHYSICS_JOINT_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

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
		PhysicsJoint(const DeviceIdentifier &id, const nlohmann::json &data);
		PhysicsJoint(const DeviceIdentifier &id, const EngineGrpc::GetDeviceMessage &data);

		float position() const;
		void setPosition(float position);

		float velocity() const;
		void setVelocity(float velocity);

		float effort() const;
		void setEffort(float effort);

		virtual void serialize(EngineGrpc::SetDeviceMessage * request) const override;
};

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PhysicsJointConst::FloatNan &property);

template<>
PhysicsJointConst::FloatNan JSONPropertySerializerMethods::deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name);


#endif // PHYSICS_JOINT_H
