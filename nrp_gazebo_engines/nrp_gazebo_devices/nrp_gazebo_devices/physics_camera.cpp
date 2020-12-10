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

#include "nrp_gazebo_devices/physics_camera.h"

uint32_t PhysicsCamera::imageHeight() const
{
	return this->getPropertyByName<ImageHeight>();
}

uint32_t &PhysicsCamera::imageHeight()
{
	return this->getPropertyByName<ImageHeight>();
}

void PhysicsCamera::setImageHeight(uint32_t height)
{
	this->getPropertyByName<ImageHeight>() = height;
}

uint32_t PhysicsCamera::imageWidth() const
{
	return this->getPropertyByName<ImageWidth>();
}

uint32_t &PhysicsCamera::imageWidth()
{
	return this->getPropertyByName<ImageWidth>();
}

void PhysicsCamera::setImageWidth(uint32_t width)
{
	this->getPropertyByName<ImageWidth>() = width;
}

uint8_t PhysicsCamera::imagePixelSize() const
{
	return this->getPropertyByName<ImagePixelSize>();
}

uint8_t &PhysicsCamera::imagePixelSize()
{
	return this->getPropertyByName<ImagePixelSize>();
}

void PhysicsCamera::setImagePixelSize(uint8_t pixel_size)
{
	this->getPropertyByName<ImagePixelSize>() = pixel_size;
}

const PhysicsCameraConst::cam_data_t &PhysicsCamera::imageData() const
{
	return this->getPropertyByName<ImageData>();
}

PhysicsCameraConst::cam_data_t &PhysicsCamera::imageData()
{
	return this->getPropertyByName<ImageData>();
}

void PhysicsCamera::setImageData(const cam_data_t &imageData)
{
	this->getPropertyByName<ImageData>() = imageData;
}
