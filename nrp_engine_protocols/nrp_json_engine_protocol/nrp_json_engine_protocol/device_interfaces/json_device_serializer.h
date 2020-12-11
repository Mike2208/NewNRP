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

#ifndef JSON_DEVICE_SERIALIZER_H
#define JSON_DEVICE_SERIALIZER_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/device_interface/device_serializer.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <nlohmann/json.hpp>

template<>
class DeviceSerializerMethods<nlohmann::json>
{
	public:
		static constexpr std::string_view JSONTypeID = "type";
		static constexpr std::string_view JSONEngineNameID = "engine_name";

		using PROP_SERIALIZER = nlohmann::json;

		using prop_deserialization_t = typename ObjectPropertySerializerMethods<PROP_SERIALIZER>::deserialization_t;
		using deserialization_t = const nlohmann::json::const_iterator&;

		template<DEVICE_C DEVICE>
		static nlohmann::json serialize(const DEVICE &dev)
		{
			nlohmann::json data = serializeID(dev.id());
			data.front() = JSONPropertySerializer<DEVICE>::serializeProperties(dev, std::move(data.front()));
			return data;
		}

		static nlohmann::json serializeID(const DeviceIdentifier &devID);

		template<DEVICE_C DEVICE>
		static DEVICE deserialize(DeviceIdentifier &&devID, deserialization_t data)
		{	return DEVICE(std::move(devID), DEVICE::deserializeProperties(data.value()));	}

		static DeviceIdentifier deserializeID(deserialization_t data);
};

using JSONDeviceSerializerMethods = DeviceSerializerMethods<nlohmann::json>;

#endif // JSON_DEVICE_SERIALIZER_H
