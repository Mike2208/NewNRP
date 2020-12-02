#include "nrp_gazebo_devices/config/cmake_constants.h"
#include "nrp_gazebo_devices/physics_camera.h"
#include "nrp_gazebo_devices/physics_joint.h"
#include "nrp_gazebo_devices/physics_link.h"

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/device_interface/python_device.h"
#include "nrp_general_library/config/cmake_constants.h"


#include <boost/python.hpp>
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
