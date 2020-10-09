#include "nrp_python_device/python_module.h"

#include "nrp_general_library/config_headers/nrp_cmake_constants.h"

#include "nrp_general_library/device_interface/devices/python_object_device_interface.h"

#include "nrp_general_library/transceiver_function/transceiver_function.h"
#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"
#include "nrp_general_library/transceiver_function/single_transceiver_device.h"

#include "nrp_general_library/device_interface/python_device.h"

#include <boost/python.hpp>

using namespace boost::python;

using DeviceIdentifiers = EngineInterface::device_identifiers_t;

struct TransceiverDeviceInterfaceWrapper
        : TransceiverDeviceInterface, wrapper<TransceiverDeviceInterface>
{
	boost::python::object runTf(boost::python::tuple &args, boost::python::dict &kwargs) override
	{
		if(override runTf = this->get_override("runTf"))
			return runTf(*args, **kwargs);

		return TransceiverDeviceInterface::runTf(args, kwargs);
	}

	boost::python::object defaultRunTf(boost::python::tuple &args, boost::python::dict &kwargs)
	{
		return TransceiverDeviceInterface::runTf(args, kwargs);
	}

	EngineInterface::device_identifiers_t getRequestedDeviceIDs() const override
	{
		if(override getReqIDs = this->get_override("_getRequestedDeviceIDs"))
			return getReqIDs();

		return TransceiverDeviceInterface::getRequestedDeviceIDs();
	}

	EngineInterface::device_identifiers_t defaultGetRequestedDeviceIDs() const
	{
		return TransceiverDeviceInterface::getRequestedDeviceIDs();
	}
};

BOOST_PYTHON_MODULE(PYTHON_MODULE_NAME)
{
	class_<DeviceIdentifier>("DeviceIdentifier", init<const std::string&, const std::string&, const std::string&>())
	        .def_readwrite("name", &DeviceIdentifier::Name)
	        .def_readwrite("type", &DeviceIdentifier::Type)
	        .def_readwrite("engine_name", &DeviceIdentifier::EngineName);

	class_<DeviceInterface>("DeviceInterface", init<const std::string &, const std::string&, const std::string&>())
	        .add_property("name", make_function(&DeviceInterface::name, return_value_policy<copy_const_reference>()), &DeviceInterface::setName)
	        .add_property("type", make_function(&DeviceInterface::type, return_value_policy<copy_const_reference>()), &DeviceInterface::setType)
	        .add_property("engine_name", make_function(&DeviceInterface::engineName, return_value_policy<copy_const_reference>()), &DeviceInterface::setEngineName)
	        .add_property("id", make_function(&DeviceInterface::id, return_value_policy<reference_existing_object>()), &DeviceInterface::setID);

	register_ptr_to_python<DeviceInterfaceSharedPtr>();
	register_ptr_to_python<DeviceInterfaceConstSharedPtr>();

	class_<PythonObjectDeviceInterface, bases<DeviceInterface> >("PythonDevice", init<const DeviceIdentifier&, boost::python::object>())
	        .add_property("data", &PythonObjectDeviceInterface::pyData, &PythonObjectDeviceInterface::setPyData);

	class_<TransceiverDeviceInterfaceWrapper, boost::noncopyable>("TransceiverDeviceInterface", init<>())
	        .def("__call__", &TransceiverDeviceInterface::pySetup<TransceiverDeviceInterface>)
	        .def("runTf", &TransceiverDeviceInterface::runTf, &TransceiverDeviceInterfaceWrapper::defaultRunTf)
	        .def("getRequestedDeviceIDs", &TransceiverDeviceInterface::getRequestedDeviceIDs, &TransceiverDeviceInterfaceWrapper::defaultGetRequestedDeviceIDs);

	register_ptr_to_python<TransceiverDeviceInterface::shared_ptr>();
	register_ptr_to_python<TransceiverDeviceInterface::const_shared_ptr>();

	class_<SingleTransceiverDevice, bases<TransceiverDeviceInterface> >("SingleTransceiverDevice", init<const std::string&, const DeviceIdentifier&>(args("keyword", "id")))
	        .def("__call__", &TransceiverDeviceInterface::pySetup<SingleTransceiverDevice>);

	class_<TransceiverFunction, bases<TransceiverDeviceInterface> >("TransceiverFunction", init<std::string>())
	        .def("__call__", &TransceiverFunction::pySetup)
	        .def("runTf", &TransceiverFunction::runTf);

	register_ptr_to_python<PtrTemplates<TransceiverFunction>::shared_ptr>();
	register_ptr_to_python<PtrTemplates<TransceiverFunction>::const_shared_ptr>();
}
