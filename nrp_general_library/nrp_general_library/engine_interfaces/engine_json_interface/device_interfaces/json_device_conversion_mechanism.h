#ifndef JSON_DEVICE_CONVERSION_MECHANISM_H
#define JSON_DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_interface.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <iostream>
#include <nlohmann/json.hpp>

template<JSON_DEVICE_C ...JSON_DEVICES>
struct DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator, JSON_DEVICES...>
{
	static constexpr std::string_view JSONTypeID = "type";
	static constexpr std::string_view JSONEngineNameID = "engine_name";

	static void init(const EngineInterface &)
	{}

	static void shutdown()
	{}

	template<JSON_DEVICE_C JSON_DEVICE>
	static nlohmann::json serialize(const JSON_DEVICE &device)
	{
		nlohmann::json data = serializeID(device.id());
		data.front() = JSONPropertySerializer<JSON_DEVICE>::serializeProperties(device, std::move(data.front()));
		return data;
	}

	template<JSON_DEVICE_C JSON_DEVICE>
	static constexpr bool IsSerializable = std::is_invocable_v<decltype(serialize<JSON_DEVICE>), const JSON_DEVICE&>;

	template<JSON_DEVICE_C JSON_DEVICE>
	static JSON_DEVICE deserialize(const nlohmann::json::const_iterator &data)
	{	return JSON_DEVICE(static_cast<const DeviceIdentifier&>(getID(data)), static_cast<const nlohmann::json&>(data.value()));	}

	template<JSON_DEVICE_C JSON_DEVICE>
	static constexpr bool IsDeserializable = std::is_invocable_v<decltype(deserialize<JSON_DEVICE>), const nlohmann::json::const_iterator&>;

	static nlohmann::json serializeID(const DeviceIdentifier &id)
	{	return nlohmann::json({{ id.Name, {{ JSONTypeID.data(), id.Type }, { JSONEngineNameID.data(), id.EngineName }} }});	}

	static DeviceIdentifier getID(const nlohmann::json::const_iterator &data)
	{
		try
		{
			const auto &dataVal = data.value();
			return DeviceIdentifier(data.key(), dataVal[JSONTypeID.data()], dataVal[JSONEngineNameID.data()]);
		}
		catch(const std::exception &e)
		{
			// TODO: Handle json parsing error
			std::cerr << e.what();
			throw;
		}
	}
};

template<JSON_DEVICE_C ...JSON_DEVICES>
using JSONDeviceConversionMechanism = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator, JSON_DEVICES...>;

#endif // JSON_DEVICE_CONVERSION_MECHANISM_H
