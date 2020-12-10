/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H
#define PYTHON_ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/devices/pyobject_device.h"
#include "nrp_json_engine_protocol/device_interfaces/json_device_serializer.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"
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
