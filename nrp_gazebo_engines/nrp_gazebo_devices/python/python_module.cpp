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

#include "nrp_gazebo_devices/config/cmake_constants.h"
#include "nrp_gazebo_devices/physics_camera.h"
#include "nrp_gazebo_devices/physics_joint.h"
#include "nrp_gazebo_devices/physics_link.h"

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/device_interface/python_device.h"
#include "nrp_general_library/config/cmake_constants.h"


#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

template<>
struct python_property<PhysicsJoint::FloatNan>
{
		static auto return_policy()
		{	return return_value_policy<copy_const_reference>();	}
};

struct FloatNanToPython
{
	public:
		static PyObject* convert(const PhysicsJoint::FloatNan &val)
		{
			return boost::python::incref(boost::python::object((float)val).ptr());
		}
};

void initFloatNanConverter()
{
	boost::python::to_python_converter<
	    PhysicsJoint::FloatNan,
	    FloatNanToPython>();
}

BOOST_PYTHON_MODULE(GAZEBO_PYTHON_MODULE_NAME)
{
	numpy::initialize();
	import(PYTHON_MODULE_NAME_STR);

	class_<typename PhysicsCamera::cam_data_t>("__CamDataVec", no_init)
	        .def(vector_indexing_suite<typename PhysicsCamera::cam_data_t>());

	initFloatNanConverter();
	implicitly_convertible<typename PhysicsJoint::FloatNan, float>();
	implicitly_convertible<float, typename PhysicsJoint::FloatNan>();

	python_property_device_class<PhysicsCamera>::create();

	python_property_device_class<PhysicsJoint>::create();

	python_property_device_class<PhysicsLink>::create();
}


/*! \defgroup gazebo_devices Gazebo Devices
 * Gazebo engines use the following devices:
 * - PhysicsCamera: Get camera image
 * - PhysicsJoint: Get/Set joint data
 * - PhysicsLink: Get link data
 */
