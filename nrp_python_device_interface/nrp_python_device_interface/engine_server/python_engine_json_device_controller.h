#ifndef PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H
#define PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/devices/python_object_device_interface.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>

template<DEVICE_C DEVICE>
class PythonEngineJSONDeviceController;

template<>
class PythonEngineJSONDeviceController<PythonObjectDeviceInterface>
        : public EngineJSONDeviceController
{
	public:
		PythonEngineJSONDeviceController(const DeviceIdentifier &devID, boost::python::object data = boost::python::object());
		virtual ~PythonEngineJSONDeviceController() override = default;

		virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator&) override;

		virtual nlohmann::json handleDeviceData(const nlohmann::json &data) override;

		/*!
		 * \brief Get python object referenced by this controller
		 */
		boost::python::object &data();

		boost::python::object data() const;

	protected:
		/*!
		 * \brief Device Data. Used to convert to/from JSON and python dict
		 */
		PythonObjectDeviceInterface _deviceData;
};

#endif // PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H
