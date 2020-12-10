//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_gazebo_mpi_engine/devices/physics_joint.h"

#include <type_traits>

PhysicsJointConst::FloatNan::FloatNan(float val)
    : _val(val)
{}

PhysicsJoint::PhysicsJoint(const std::string &name)
    : PhysicsJoint(DeviceIdentifier(name, "", TypeName.data()))
{}

PhysicsJoint::PhysicsJoint(const DeviceIdentifier &id)
    : DeviceInterface(id),
      PropertyTemplate(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN())
{}

float PhysicsJoint::position() const
{
	return this->getPropertyByName<Position>();
}

void PhysicsJoint::setPosition(float position)
{
	this->getPropertyByName<Position>() = position;
}

float PhysicsJoint::velocity() const
{
	return this->getPropertyByName<Velocity>();
}

void PhysicsJoint::setVelocity(float velocity)
{
	this->getPropertyByName<Velocity>() = velocity;
}

float PhysicsJoint::effort() const
{
	return this->getPropertyByName<Effort>();
}

void PhysicsJoint::setEffort(float effort)
{
	this->getPropertyByName<Effort>() = effort;
}
