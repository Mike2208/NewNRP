#ifndef NEST_JSON_DEVICE_INTERFACE_H
#define NEST_JSON_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/devices/python_object_device_interface.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>

/*!
 *	\brief NEST JSON Device
 *	\tparam DEVICE Final Derived class
 */
class NestJSONDeviceInterface
        : public PythonObjectDeviceInterface
{
	public:		
		/*!
		 *	\brief Constructor
		 *	\param devID Device ID
		 *	\param json Device Data, in JSON format
		 */
		NestJSONDeviceInterface(const DeviceIdentifier &devID, const nlohmann::json &json);

		/*!
		 *	\brief Constructor
		 *	\param devID Device ID
		 *	\param data Device Data, as python dict
		 */
		NestJSONDeviceInterface(const DeviceIdentifier &devID, const boost::python::dict &data);

		virtual ~NestJSONDeviceInterface() override = default;

		boost::python::dict data() const;

	private:
};

#endif // NEST_JSON_DEVICE_INTERFACE_H
