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
