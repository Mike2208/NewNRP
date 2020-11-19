#include "nrp_python_device/python_module.h"

#include "nrp_general_library/config/cmake_constants.h"

#include "nrp_general_library/device_interface/devices/pyobject_device.h"

#include "nrp_general_library/transceiver_function/transceiver_function.h"
#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"
#include "nrp_general_library/transceiver_function/single_transceiver_device.h"

#include "nrp_general_library/device_interface/python_device.h"

#include <boost/python.hpp>

namespace python = boost::python;

using DeviceIdentifiers = EngineInterface::device_identifiers_t;

struct TransceiverDeviceInterfaceWrapper
        : TransceiverDeviceInterface, python::wrapper<TransceiverDeviceInterface>
{
	python::object runTf(python::tuple &args, python::dict &kwargs) override
	{
		if(python::override runTf = this->get_override("runTf"))
			return runTf(*args, **kwargs);

		return TransceiverDeviceInterface::runTf(args, kwargs);
	}

	python::object defaultRunTf(python::tuple &args, python::dict &kwargs)
	{
		return TransceiverDeviceInterface::runTf(args, kwargs);
	}

	EngineInterface::device_identifiers_t getRequestedDeviceIDs() const override
	{
		if(python::override getReqIDs = this->get_override("_getRequestedDeviceIDs"))
			return getReqIDs();

		return TransceiverDeviceInterface::getRequestedDeviceIDs();
	}

	EngineInterface::device_identifiers_t defaultGetRequestedDeviceIDs() const
	{
		return TransceiverDeviceInterface::getRequestedDeviceIDs();
	}
};

inline std::shared_ptr<DeviceIdentifier> genDevID(const std::string &name, const std::string &engineName)
{
	return std::shared_ptr<DeviceIdentifier>(new DeviceIdentifier(name, engineName, ""));
}

inline python::object getPyPropData(const PyObjectDeviceConst::PyObjData &dat)
{	return dat;	}

inline void setPyPropData(PyObjectDeviceConst::PyObjData &dat, const python::object &val)
{	dat = val;	}


inline python::object getPyDevData(const PyObjectDevice &dat)
{	return dat.data();	}

inline void setPyDevData(PyObjectDevice &dat, const python::object &val)
{	dat.data() = val;	}


using namespace boost::python;

BOOST_PYTHON_MODULE(PYTHON_MODULE_NAME)
{
	class_<DeviceIdentifier>("DeviceIdentifier", init<const std::string&, const std::string &, const std::string&>((arg("name"), arg("engine_name"), arg("type") = std::string())))
	        .def("__init__", make_constructor(&genDevID))
	        .def_readwrite("name", &DeviceIdentifier::Name)
	        .def_readwrite("type", &DeviceIdentifier::Type)
	        .def_readwrite("engine_name", &DeviceIdentifier::EngineName);

	register_ptr_to_python<std::shared_ptr<DeviceIdentifier> >();
	register_ptr_to_python<std::shared_ptr<const DeviceIdentifier> >();

	class_<DeviceInterface>("DeviceInterface", init<const std::string &, const std::string&, const std::string&>())
	        .add_property("name", make_function(&DeviceInterface::name, return_value_policy<copy_const_reference>()), &DeviceInterface::setName)
	        .add_property("type", make_function(&DeviceInterface::type, return_value_policy<copy_const_reference>()), &DeviceInterface::setType)
	        .add_property("engine_name", make_function(&DeviceInterface::engineName, return_value_policy<copy_const_reference>()), &DeviceInterface::setEngineName)
	        .add_property("id", make_function(&DeviceInterface::id, return_value_policy<reference_existing_object>()), &DeviceInterface::setID);

	register_ptr_to_python<DeviceInterfaceSharedPtr>();
	register_ptr_to_python<DeviceInterfaceConstSharedPtr>();

	class_<PyObjectDeviceConst::PyObjData>("PyObjData", init<boost::python::object>())
	        .add_property("data", &getPyPropData, &setPyPropData);

	python_property_device_class<PyObjectDevice>::create()
	        .add_property(PyObjectDevice::Object.m_data, &getPyDevData, &setPyDevData);

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
