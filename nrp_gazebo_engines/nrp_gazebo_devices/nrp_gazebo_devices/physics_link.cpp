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

#include "nrp_gazebo_devices/physics_link.h"


const PhysicsLink::vec3_t &PhysicsLink::position() const
{
	return this->getPropertyByName<Position>();
}

void PhysicsLink::setPosition(const PhysicsLink::vec3_t &position)
{
	this->getPropertyByName<Position>() = position;
}

const PhysicsLink::quat_t &PhysicsLink::rotation() const
{
	return this->getPropertyByName<Rotation>();
}

void PhysicsLink::setRotation(const PhysicsLink::quat_t &rotation)
{
	this->getPropertyByName<Rotation>() = rotation;
}

const PhysicsLink::vec3_t &PhysicsLink::linVel() const
{
	return this->getPropertyByName<LinearVelocity>();
}

void PhysicsLink::setLinVel(const PhysicsLink::vec3_t &linVel)
{
	this->getPropertyByName<LinearVelocity>() = linVel;
}

const PhysicsLink::vec3_t &PhysicsLink::angVel() const
{
	return this->getPropertyByName<AngularVelocity>();
}

void PhysicsLink::setAngVel(const PhysicsLink::vec3_t &angVel)
{
	this->getPropertyByName<AngularVelocity>() = angVel;
}
