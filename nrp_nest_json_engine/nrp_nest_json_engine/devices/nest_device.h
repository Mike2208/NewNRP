#ifndef NEST_DEVICE_H
#define NEST_DEVICE_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"
#include "nrp_nest_json_engine/config/cmake_constants.h"

#include <boost/python.hpp>

/*!
 *	\brief NEST Device
 */
class NestDevice
        : public Device<NestDevice, "NestDevice", PropNames<"data">, PyObjectDeviceConst::PyObjData>
{
		static auto defaultPyObject(const boost::python::object &data = boost::python::dict())
		{	return PyObjectDeviceConst::PyObjData(data, boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("__dict__")["NumpyEncoder"]);	}

	public:
		NestDevice(DeviceIdentifier &&devID, const boost::python::object &data = boost::python::dict());
		NestDevice(DeviceIdentifier &&devID, property_template_t &&props);

		template<class DESERIALIZE_T>
		static auto deserializeProperties(DESERIALIZE_T &&data)
		{
			return Device::deserializeProperties(std::forward<DESERIALIZE_T>(data), defaultPyObject());
		}

		const boost::python::object &data() const;
		boost::python::object &data();
};

#endif // NEST_DEVICE_H
