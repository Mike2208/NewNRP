#ifndef NEST_DEVICE_H
#define NEST_DEVICE_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>

/*!
 *	\brief NEST Device
 */
class NestDevice
        : public Device<NestDevice, "NestDevice", PropNames<"data">, PyObjectDeviceConst::PyObjData>
{
	public:
		NestDevice(DeviceIdentifier &&devID, const boost::python::object &data = boost::python::dict());

		template<class DESERIALIZE_T>
		static NestDevice deserialize(DeviceIdentifier &&devID, DESERIALIZE_T &&data)
		{	return Device::deserialize(std::move(devID), std::forward<DESERIALIZE_T>(data), boost::python::dict());	}

		const boost::python::object &data() const;
		boost::python::object &data();

	private:
};

#endif // NEST_DEVICE_H
