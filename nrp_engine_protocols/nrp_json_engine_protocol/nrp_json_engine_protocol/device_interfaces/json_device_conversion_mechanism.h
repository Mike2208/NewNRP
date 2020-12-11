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

#ifndef JSON_DEVICE_CONVERSION_MECHANISM_H
#define JSON_DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"
#include "nrp_json_engine_protocol/device_interfaces/json_device_serializer.h"
#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <iostream>
#include <nlohmann/json.hpp>

/*!
 *  \brief Converts Devices to/from the JSON format
 */
template<DEVICE_C ...DEVICES>
struct DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator, DEVICES...>
{
	static constexpr std::string_view JSONTypeID = "type";
	static constexpr std::string_view JSONEngineNameID = "engine_name";

	static void init(const EngineInterface &)
	{}

	static void shutdown()
	{}

	template<DEVICE_C DEVICE>
	static nlohmann::json serialize(const DEVICE &device)
	{
		nlohmann::json data = serializeID(device.id());
		data.front() = JSONPropertySerializer<DEVICE>::serializeProperties(device, std::move(data.front()));
		return data;
	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsSerializable = std::is_invocable_v<decltype(serialize<DEVICE>), const DEVICE&>;

	template<DEVICE_C DEVICE>
	static DEVICE deserialize(const nlohmann::json::const_iterator &data)
	{	return DEVICE(getID(data),  DEVICE::deserializeProperties(data.value()));	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsDeserializable = std::is_invocable_v<decltype(deserialize<DEVICE>), const nlohmann::json::const_iterator&>;

	static nlohmann::json serializeID(const DeviceIdentifier &id)
	{	return nlohmann::json({{ id.Name, {{ JSONTypeID.data(), id.Type }, { JSONEngineNameID.data(), id.EngineName }} }});	}

	static DeviceIdentifier getID(const nlohmann::json::const_iterator &data)
	{
		try
		{
			const auto &dataVal = data.value();
			return DeviceIdentifier(data.key(), dataVal[JSONEngineNameID.data()], dataVal[JSONTypeID.data()]);
		}
		catch(std::exception &e)
		{
			// TODO: Handle json parsing error
			throw NRPException::logCreate(e, "Failed to parse JSON DeviceID");
		}
	}
};

template<DEVICE_C ...DEVICES>
using JSONDeviceConversionMechanism = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator, DEVICES...>;

#endif // JSON_DEVICE_CONVERSION_MECHANISM_H
