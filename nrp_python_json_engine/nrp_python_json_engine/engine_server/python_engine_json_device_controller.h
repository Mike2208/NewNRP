#ifndef PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H
#define PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_serializer.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>

template<class DEVICE>
class PythonEngineJSONDeviceController;

template<>
class PythonEngineJSONDeviceController<PyObjectDevice>
        : public EngineDeviceController<nlohmann::json, PyObjectDevice>
{
	public:
		PythonEngineJSONDeviceController(DeviceIdentifier &&devID, boost::python::object data = boost::python::object());

		virtual ~PythonEngineJSONDeviceController() override = default;

		virtual void handleDeviceDataCallback(PyObjectDevice &&data) override;
		virtual const PyObjectDevice *getDeviceInformationCallback() override;

		/*!
		 * \brief Get python object referenced by this controller
		 */
		boost::python::object &data();

		boost::python::object data() const;

	protected:
		/*!
		 * \brief Device Data. Used to convert to/from JSON and python dict
		 */
		PyObjectDevice _deviceData;
};

#endif // PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H
