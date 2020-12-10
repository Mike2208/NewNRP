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
