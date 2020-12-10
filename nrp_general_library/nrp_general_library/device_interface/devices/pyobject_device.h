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

#ifndef PYOBJECT_DEVICE_H
#define PYOBJECT_DEVICE_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <boost/python.hpp>

struct PyObjectDeviceConst
{
	struct PyObjData
	        : public boost::python::object
	{
		boost::python::object JsonEncoder = defaultEncoder();
		boost::python::object JsonDecoder = defaultDecoder();

		PyObjData() = default;
		PyObjData(const std::string &serializedData, boost::python::object _jsonEncoder = PyObjData::defaultEncoder(), boost::python::object _jsonDecoder = PyObjData::defaultDecoder());
		PyObjData(boost::python::object _data, boost::python::object _jsonEncoder = PyObjData::defaultEncoder(), boost::python::object _jsonDecoder = PyObjData::defaultDecoder());

		std::string serialize() const;
		boost::python::object deserialize(const std::string &serializedData);

		private:
		    static boost::python::object defaultEncoder();
			static boost::python::object defaultDecoder();
	};

	static constexpr FixedString Object = "data";

	using PyPropNames = PropNames<Object>;
};

class PyObjectDevice
        : public Device<PyObjectDevice, "PythonDevice", PyObjectDeviceConst::PyPropNames,
                        PyObjectDeviceConst::PyObjData>,
          public PyObjectDeviceConst
{
	public:
		PyObjectDevice(const DeviceIdentifier &id, const nlohmann::json &data);

		template<class DEVID_T, class ...DATA_T>
		PyObjectDevice(DEVID_T &&id, DATA_T &&...data)
		    : Device(std::forward<DEVID_T>(id), std::forward<DATA_T>(data)...)
		{}

		const PyObjData &data() const;
		PyObjData &data();

		void setData(const PyObjData &data);

		boost::python::object pyData() const;
		void setPyData(boost::python::object data);
};

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PyObjectDeviceConst::PyObjData &property);

template<>
PyObjectDeviceConst::PyObjData JSONPropertySerializerMethods::deserializeSingleProperty<PyObjectDeviceConst::PyObjData>(const nlohmann::json &data, const std::string_view &name);

#endif // PYOBJECT_DEVICE_H
