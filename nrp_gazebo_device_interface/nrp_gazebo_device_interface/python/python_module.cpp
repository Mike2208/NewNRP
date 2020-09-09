#include "nrp_gazebo_device_interface/devices/json_physics_camera.h"
#include "nrp_gazebo_device_interface/devices/json_physics_joint.h"
#include "nrp_gazebo_device_interface/devices/json_physics_link.h"

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/config_headers/nrp_cmake_constants.h"

#include "nrp_gazebo_device_interface/config/nrp_gazebo_cmake_constants.h"

#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(GAZEBO_PYTHON_MODULE_NAME)
{
	constexpr auto (JSONPhysicsCamera::*imageHeight)() const  = &JSONPhysicsCamera::imageHeight;
	constexpr auto (JSONPhysicsCamera::*imageWidth)() const  = &JSONPhysicsCamera::imageWidth;
	constexpr auto (JSONPhysicsCamera::*imageDepth)() const  = &JSONPhysicsCamera::imagePixelSize;
	constexpr auto (JSONPhysicsCamera::*imageData)() const  = &JSONPhysicsCamera::imageData;

	class_<JSONPhysicsCamera, bases<DeviceInterface> >("JSONPhysicsCamera", init<const std::string &>())
	        .add_property("image_height", imageHeight, &JSONPhysicsCamera::setImageHeight)
	        .add_property("image_idth", imageWidth, &JSONPhysicsCamera::setImageWidth)
	        .add_property("image_depth", imageDepth, &JSONPhysicsCamera::setImagePixelSize)
	        .add_property("image_data", make_function(imageData, return_internal_reference<>()), &JSONPhysicsCamera::setImageData);

	register_ptr_to_python<std::shared_ptr<JSONPhysicsCamera> >();
	register_ptr_to_python<std::shared_ptr<const JSONPhysicsCamera> >();


	class_<PhysicsJoint, bases<DeviceInterface> >("PhysicsJoint", init<const std::string &>())
	        .add_property("position", &PhysicsJoint::position, &PhysicsJoint::setPosition)
	        .add_property("velocity", &PhysicsJoint::velocity, &PhysicsJoint::setVelocity)
	        .add_property("acceleration", &PhysicsJoint::effort, &PhysicsJoint::setEffort);

	register_ptr_to_python<std::shared_ptr<PhysicsJoint> >();
	register_ptr_to_python<std::shared_ptr<const PhysicsJoint> >();


	class_<PhysicsJoint, bases<DeviceInterface> >("PhysicsJoint", init<const std::string &>())
	        .add_property("effort", &PhysicsJoint::effort, &PhysicsJoint::setEffort);

	register_ptr_to_python<std::shared_ptr<PhysicsJoint> >();
	register_ptr_to_python<std::shared_ptr<const PhysicsJoint> >();
}
