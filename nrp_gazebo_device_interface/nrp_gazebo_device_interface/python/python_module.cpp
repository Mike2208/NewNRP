#include "nrp_gazebo_device_interface/devices/physics_camera.h"
#include "nrp_gazebo_device_interface/devices/physics_joint.h"
#include "nrp_gazebo_device_interface/devices/physics_link.h"

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/config_headers/nrp_cmake_constants.h"

#include "nrp_gazebo_device_interface/config/nrp_gazebo_cmake_constants.h"

#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(GAZEBO_PYTHON_MODULE_NAME)
{
	constexpr auto (PhysicsCamera::*imageHeight)() const  = &PhysicsCamera::imageHeight;
	constexpr auto (PhysicsCamera::*imageWidth)() const  = &PhysicsCamera::imageWidth;
	constexpr auto (PhysicsCamera::*imageDepth)() const  = &PhysicsCamera::imagePixelSize;
	constexpr auto (PhysicsCamera::*imageData)() const  = &PhysicsCamera::imageData;

	class_<PhysicsCamera, bases<DeviceInterface> >("PhysicsCamera", init<const std::string &>())
	        .add_property("image_height", imageHeight, &PhysicsCamera::setImageHeight)
	        .add_property("image_idth", imageWidth, &PhysicsCamera::setImageWidth)
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


	class_<PhysicsJoint, bases<DeviceInterface> >("PhysicsJoint", init<const std::string &>())
	        .add_property("effort", &PhysicsJoint::effort, &PhysicsJoint::setEffort);

	register_ptr_to_python<std::shared_ptr<PhysicsJoint> >();
	register_ptr_to_python<std::shared_ptr<const PhysicsJoint> >();
}
