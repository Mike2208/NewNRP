#ifndef NEST_SERVER_DEVICE_H
#define NEST_SERVER_DEVICE_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>

/*!
 *	\brief NEST Device
 */
class NestServerDevice
        : public Device<NestServerDevice, "nest_server_dev", PropNames<"cmd", "args", "kwargs", "data">,
                        std::string, PyObjectDeviceConst::PyObjData,
                        PyObjectDeviceConst::PyObjData, PyObjectDeviceConst::PyObjData>
{
	public:
		using PyObjData = PyObjectDeviceConst::PyObjData;

		/*!
		 * \brief Constructor
		 * \param devID Device ID
		 * \param json Device Data, in JSON format
		 */
		NestServerDevice(const DeviceIdentifier &devID, const nlohmann::json &json);

		/*!
		 * \brief Constructor
		 * \param devID Device ID
		 * \param cmd Nest Command to execute
		 * \param args Nest Command arguments
		 * \param kwargs Nest Command keyword arguments
		 */
		NestServerDevice(const DeviceIdentifier &devID, const std::string &cmd, const boost::python::list &args, const boost::python::dict &kwargs);

		/*!
		 * \brief Constructor
		 * \param devID Device ID
		 * \param data Device Data, as python dict
		 */
		NestServerDevice(const DeviceIdentifier &devID, const boost::python::object &data = boost::python::object());

		virtual ~NestServerDevice() override = default;

		const std::string &cmd() const;
		std::string &cmd();

		const PyObjData &args() const;
		PyObjData &args();

		const PyObjData &kwargs() const;
		PyObjData &kwargs();

		const PyObjData &data() const;
		PyObjData &data();

	private:
};

#endif // NEST_SERVER_DEVICE_H
