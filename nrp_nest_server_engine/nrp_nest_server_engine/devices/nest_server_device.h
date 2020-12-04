#ifndef NEST_SERVER_DEVICE_H
#define NEST_SERVER_DEVICE_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"
#include "nrp_nest_server_engine/config/cmake_constants.h"

#include <boost/python.hpp>

/*!
 *	\brief NEST Device
 */
class NestServerDevice
        : public Device<NestServerDevice, "NestServerDevice", PropNames<"data">, PyObjectDeviceConst::PyObjData>
{
	public:
		using PyObjData = PyObjectDeviceConst::PyObjData;

		NestServerDevice(DeviceIdentifier &&devID, const std::string &data);

		NestServerDevice(DeviceIdentifier &&devID, property_template_t &&data = property_template_t());

		template<class SERIALIZER>
		static auto deserializeProperties(SERIALIZER &&data)
		{
			property_template_t props = Device::deserializeProperties(std::forward<SERIALIZER>(data));
			props.getPropertyByName<"data">().JsonEncoder = boost::python::import(NRP_NEST_PYTHON_MODULE_STR).attr("NumpyEncoder");
			return props;
		}

		constexpr const PyObjData &data() const
		{	return this->getPropertyByName<"data">();	}
		constexpr PyObjData &data()
		{	return this->getPropertyByName<"data">();	}

	private:
};

#endif // NEST_SERVER_DEVICE_H
