#ifndef NEST_JSON_DEVICE_INTERFACE_H
#define NEST_JSON_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/devices/python_object_device_interface.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>

/*!
 *	\brief NEST Device
 */
class NestDeviceInterface
        : public PythonObjectDeviceInterface
{
	public:
		static constexpr std::string_view TypeName = "nest_dev";

		/*!
		 *	\brief Constructor
		 *	\param devID Device ID
		 *	\param json Device Data, in JSON format
		 */
		NestDeviceInterface(const DeviceIdentifier &devID, const nlohmann::json &json);

		/*!
		 *	\brief Constructor
		 *	\param devID Device ID
		 *	\param data Device Data, as python dict
		 */
		NestDeviceInterface(const DeviceIdentifier &devID, const boost::python::object &data = boost::python::object());

		virtual ~NestDeviceInterface() override = default;

		const boost::python::object &data() const;
		boost::python::object &data();

	private:
};

#endif // NEST_JSON_DEVICE_INTERFACE_H
