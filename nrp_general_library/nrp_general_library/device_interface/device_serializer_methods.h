#ifndef DEVICE_SERIALIZER_METHODS_H
#define DEVICE_SERIALIZER_METHODS_H

#include "nrp_general_library/device_interface/device_serializer.h"

template<class SERIALIZER>
class DeviceSerializerMethods
{
	public:
		using PROP_SERIALIZER = SERIALIZER;

		using prop_deserialization_t = typename ObjectPropertySerializerMethods<PROP_SERIALIZER>::deserialization_t;
		using deserializtion_t = const SERIALIZER&;

		template<DEVICE_C DEVICE>
		static SERIALIZER serialize(const DEVICE &dev);

		static SERIALIZER serializeID(const DeviceIdentifier &devID);

		template<DEVICE_C DEVICE>
		static DEVICE deserialize(deserializtion_t data);

		static DeviceIdentifier deserializeID(deserializtion_t data);
};

#endif // DEVICE_SERIALIZER_METHODS_H
