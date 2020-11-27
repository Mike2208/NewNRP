#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_serializer.h"

nlohmann::json DeviceSerializerMethods<nlohmann::json>::serializeID(const DeviceIdentifier &devID)
{	return nlohmann::json({{ devID.Name, {{ JSONTypeID.data(), devID.Type }, { JSONEngineNameID.data(), devID.EngineName }} }});	}

DeviceIdentifier DeviceSerializerMethods<nlohmann::json>::deserializeID(deserializtion_t data)
{
	try
	{
		const auto &dataVal = data.value();
		return DeviceIdentifier(data.key(), dataVal[JSONEngineNameID.data()], dataVal[JSONTypeID.data()]);
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Failed to parse JSON DeviceID");
	}
}
