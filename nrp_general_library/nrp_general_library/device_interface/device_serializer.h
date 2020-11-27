#ifndef DEVICE_SERIALIZER_H
#define DEVICE_SERIALIZER_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <concepts>

template<class SERIALIZER>
class DeviceSerializerMethods;

/*!
 *  \brief Device serializer concept. Defines necessary functions to de-/serialize devices. Uses PropertySerializers in the background
 */
template<class T, class SERIALIZER_T, class DEVICE>
concept DEVICE_SERIALIZER_METHODS_C = requires (T &serializer, const DEVICE &device)
{
    {	T::PROP_SERIALIZER	};
    {	T::prop_deserialization_t	};
    {	T::deserialization_t	};

    {	serializer.serialize(device)	} -> std::convertible_to<SERIALIZER_T>;
    {	serializer.serializeID(std::declval<const DeviceIdentifier&>())	} -> std::convertible_to<SERIALIZER_T>;

    {	serializer.deserialize(std::declval<DeviceIdentifier&&>(), std::declval<typename T::deserialization_t>())	} -> std::same_as<DEVICE>;
    {	serializer.deserializeID(std::declval<typename T::deserialization_t>())	} -> std::convertible_to<DeviceIdentifier>;
};

template<class SERIALIZER, class DEVICE>
class DeviceSerializer
{
	public:
		static_assert (DEVICE_SERIALIZER_METHODS_C<DeviceSerializerMethods<SERIALIZER>, SERIALIZER, DEVICE>,
		    "SERIALIZER serialization type does not have a valid DeviceSerializerMethods struct available");

		template<class DEVICE_T>
		requires(std::same_as<std::remove_cvref_t<DEVICE_T>, DEVICE>)
		static SERIALIZER serialize(DEVICE_T &&dev)
		{	return DeviceSerializerMethods<SERIALIZER>::serialize(std::forward<DEVICE_T>(dev));	}

		static SERIALIZER serializeID(const DeviceIdentifier &devID)
		{	return DeviceSerializerMethods<SERIALIZER>::serializeID(devID);	}

		template<class DEVICE_T>
		requires(std::same_as<std::remove_cvref_t<DEVICE_T>, DEVICE>)
		static DEVICE deserialize(DeviceIdentifier &&devID, DEVICE_T &&dev)
		{	return DeviceSerializerMethods<SERIALIZER>::deserialize(std::move(devID), std::forward<DEVICE_T>(dev));	}

		template<class SERIALIZER_T>
		requires(std::same_as<std::remove_cvref_t<SERIALIZER_T>, SERIALIZER>)
		static DeviceIdentifier serializeID(SERIALIZER_T &&devID)
		{	return DeviceSerializerMethods<SERIALIZER>::serializeID(std::forward<SERIALIZER_T>(devID));	}
};

#endif // DEVICE_SERIALIZER_H
