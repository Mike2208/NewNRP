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

#include "nrp_gazebo_mpi_engine/devices/physics_camera.h"
#include "nrp_gazebo_mpi_engine/devices/physics_joint.h"
#include "nrp_gazebo_mpi_engine/devices/physics_link.h"

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/config/cmake_constants.h"

#include "nrp_gazebo_mpi_engine/config/cmake_constants.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(GAZEBO_PYTHON_MODULE_NAME)
{
	import(PYTHON_MODULE_NAME_STR);

	constexpr auto (PhysicsCamera::*imageHeight)() const  = &PhysicsCamera::imageHeight;
	constexpr auto (PhysicsCamera::*imageWidth)() const  = &PhysicsCamera::imageWidth;
	constexpr auto (PhysicsCamera::*imageDepth)() const  = &PhysicsCamera::imagePixelSize;
	constexpr const auto &(PhysicsCamera::*imageData)() const  = &PhysicsCamera::imageData;

	class_<typename PhysicsCamera::cam_data_t>("__CamDataVec", no_init)
	        .def(vector_indexing_suite<typename PhysicsCamera::cam_data_t>());

	class_<PhysicsCamera, bases<DeviceInterface> >("PhysicsCamera", init<const std::string &>())
	        .add_property("image_height", imageHeight, &PhysicsCamera::setImageHeight)
	        .add_property("image_width", imageWidth, &PhysicsCamera::setImageWidth)
	        .add_property("image_depth", imageDepth, &PhysicsCamera::setImagePixelSize)
	        .add_property("image_data", make_function(imageData, return_internal_reference<>()), &PhysicsCamera::setImageData);

	register_ptr_to_python<std::shared_ptr<PhysicsCamera> >();
	register_ptr_to_python<std::shared_ptr<const PhysicsCamera> >();


	class_<PhysicsJoint, bases<DeviceInterface> >("PhysicsJoint", init<const std::string &>())
	        .add_property("position", &PhysicsJoint::position, &PhysicsJoint::setPosition)
	        .add_property("velocity", &PhysicsJoint::velocity, &PhysicsJoint::setVelocity)
	        .add_property("acceleration", &PhysicsJoint::effort, &PhysicsJoint::setEffort);

	register_ptr_to_python<std::shared_ptr<PhysicsJoint> >();
	register_ptr_to_python<std::shared_ptr<const PhysicsJoint> >();
}
