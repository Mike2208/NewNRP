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
		static PyObjectDeviceConst::PyObjData defaultPyObject(const boost::python::object &data = boost::python::dict())
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
