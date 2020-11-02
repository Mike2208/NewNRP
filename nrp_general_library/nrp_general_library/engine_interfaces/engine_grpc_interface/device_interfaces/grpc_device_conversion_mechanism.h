#ifndef GRPC_DEVICE_CONVERSION_MECHANISM_H
#define GRPC_DEVICE_CONVERSION_MECHANISM_H

#include <engine_grpc.grpc.pb.h>

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"

template<DEVICE_C ...DEVICES>
struct DeviceConversionMechanism<EngineGrpc::SetDeviceMessage, const EngineGrpc::GetDeviceMessage, DEVICES...>
{
    static constexpr std::string_view GrpcTypeID = "type";
    static constexpr std::string_view GrpcEngineNameID = "engine_name";

    static void init(const EngineInterface &)
    {}

    static void shutdown()
    {}

    template<DEVICE_C DEVICE>
    static EngineGrpc::SetDeviceMessage serialize(const DEVICE &device)
    {
        EngineGrpc::SetDeviceMessage message;
        message.mutable_deviceid()->CopyFrom(serializeID(device.id()));
        //data.front() = JSONPropertySerializer<DEVICE>::serializeProperties(device, std::move(data.front()));
        return message;
    }

    template<DEVICE_C DEVICE>
    static constexpr bool IsSerializable = std::is_invocable_v<decltype(serialize<DEVICE>), const DEVICE&>;

    template<DEVICE_C DEVICE>
    static DEVICE deserialize(const EngineGrpc::GetDeviceMessage &data)
    {
        return DEVICE(static_cast<const DeviceIdentifier&>(getID(data)));
    }

    template<DEVICE_C DEVICE>
    static constexpr bool IsDeserializable = std::is_invocable_v<decltype(deserialize<DEVICE>)>;

    static EngineGrpc::DeviceIdentifier serializeID(const DeviceIdentifier &id)
    {
        EngineGrpc::DeviceIdentifier message;

        message.set_devicename(id.Name);
        message.set_devicetype(id.Type);
        message.set_enginename(id.EngineName);

        return message;
    }

    static DeviceIdentifier getID(const EngineGrpc::GetDeviceMessage &data)
    {
        return DeviceIdentifier(data.deviceid().devicename(), data.deviceid().devicetype(), data.deviceid().enginename());
    }
};

template<DEVICE_C ...DEVICES>
using GrpcDeviceConversionMechanism = DeviceConversionMechanism<EngineGrpc::SetDeviceMessage, const EngineGrpc::GetDeviceMessage, DEVICES...>;

#endif // GRPC_DEVICE_CONVERSION_MECHANISM_H

// EOF