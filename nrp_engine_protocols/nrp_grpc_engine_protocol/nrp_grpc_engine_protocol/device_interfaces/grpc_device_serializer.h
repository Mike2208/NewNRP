#ifndef GRPC_DEVICE_SERIALIZER_H
#define GRPC_DEVICE_SERIALIZER_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/device_interface/device_serializer.h"

//#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.grpc.pb.h"
#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.pb.h"

class GRPCDevice
{
	public:
		GRPCDevice()
		{
			this->_dev.Clear();
		}

		template<class ...T>
		GRPCDevice(T &&...data)
		    : _dev(std::forward<T>(data)...)
		{}

		constexpr operator const auto&() const
		{	return this->_dev;	}

		constexpr operator auto&()
		{	return this->_dev;	}

		constexpr const auto &dev() const
		{	return this->_dev;	}

		constexpr auto &dev()
		{	return this->_dev;	}

	private:
		EngineGrpc::DeviceMessage _dev;
};

template<>
class DeviceSerializerMethods<GRPCDevice>
{
	public:
		using PROP_SERIALIZER = EngineGrpc::DeviceMessage;

		using prop_deserialization_t = const EngineGrpc::DeviceMessage*;
		using deserialization_t = const EngineGrpc::DeviceMessage*;

		template<DEVICE_C DEVICE>
		static GRPCDevice serialize(const DEVICE &dev);

		template<class SERIALIZER_T = GRPCDevice>
		static SERIALIZER_T serializeID(const DeviceIdentifier &devID);

		template<DEVICE_C DEVICE>
		static DEVICE deserialize(DeviceIdentifier &&devID, deserialization_t data);

		static DeviceIdentifier deserializeID(deserialization_t data);
};

template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serializeID<GRPCDevice>(const DeviceIdentifier &devID);

template<>
EngineGrpc::DeviceMessage DeviceSerializerMethods<GRPCDevice>::serializeID<EngineGrpc::DeviceMessage>(const DeviceIdentifier &devID);

using GRPCDeviceSerializerMethods = DeviceSerializerMethods<GRPCDevice>;

#endif // GRPC_DEVICE_SERIALIZER_H
