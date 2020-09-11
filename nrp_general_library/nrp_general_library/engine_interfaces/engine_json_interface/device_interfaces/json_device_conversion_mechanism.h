#ifndef JSON_DEVICE_CONVERSION_MECHANISM_H
#define JSON_DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <iostream>
#include <nlohmann/json.hpp>

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
	{	return DEVICE(static_cast<const DeviceIdentifier&>(getID(data)), static_cast<const nlohmann::json&>(data.value()));	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsDeserializable = std::is_invocable_v<decltype(deserialize<DEVICE>), const nlohmann::json::const_iterator&>;

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

template<DEVICE_C ...DEVICES>
using JSONDeviceConversionMechanism = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator, DEVICES...>;

#endif // JSON_DEVICE_CONVERSION_MECHANISM_H
