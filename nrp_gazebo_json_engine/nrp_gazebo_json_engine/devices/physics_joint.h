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

	using JPropNames = PropNames<Position, Velocity, Effort>;
};

/*!
 * \brief Joint Device
 */
class PhysicsJoint
        : public PhysicsJointConst,
          public Device<PhysicsJoint, "PhysicsJoint", PhysicsJointConst::JPropNames, PhysicsJointConst::FloatNan, PhysicsJointConst::FloatNan, PhysicsJointConst::FloatNan>
{
	public:
		PhysicsJoint(DeviceIdentifier &&devID, property_template_t &&props = property_template_t(NAN, NAN, NAN))
		    : Device(std::move(devID), std::move(props))
		{}

		template<class DESERIALIZE_T>
		static auto deserializeProperties(DESERIALIZE_T &&data)
		{	return Device::deserializeProperties(std::forward<DESERIALIZE_T>(data), NAN, NAN, NAN);	}

		float position() const;
		void setPosition(float position);

		float velocity() const;
		void setVelocity(float velocity);

		float effort() const;
		void setEffort(float effort);
};

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PhysicsJointConst::FloatNan &property);

template<>
PhysicsJointConst::FloatNan JSONPropertySerializerMethods::deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name);


#endif // PHYSICS_JOINT_H
