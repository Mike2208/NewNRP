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
concept DEVICE_SERIALIZER_METHODS_C = requires (T &serializer, const DEVICE &device, DeviceIdentifier &&id)
{
    DEVICE_C<DEVICE>;

    typename T::PROP_SERIALIZER;
    typename T::prop_deserialization_t;
    typename T::deserialization_t;

    {	serializer.serialize(device)	} -> std::convertible_to<SERIALIZER_T>;
    {	serializer.serializeID(std::declval<const DeviceIdentifier&>())	} -> std::convertible_to<SERIALIZER_T>;

    {	serializer.template deserialize<DEVICE>(std::move(id), std::declval<typename T::deserialization_t>())	} -> std::same_as<DEVICE>;
    {	serializer.deserializeID(std::declval<typename T::deserialization_t>())	} -> std::convertible_to<DeviceIdentifier>;
};

//template<class SERIALIZER>
//class DeviceSerializerMethods
//{
//	public:
//		using PROP_SERIALIZER = SERIALIZER;

//		using prop_deserialization_t = typename ObjectPropertySerializerMethods<PROP_SERIALIZER>::deserialization_t;
//		using deserializtion_t = const SERIALIZER&;

//		template<DEVICE_C DEVICE>
//		static SERIALIZER serialize(const DEVICE &dev);

//		static SERIALIZER serializeID(const DeviceIdentifier &devID);

//		template<DEVICE_C DEVICE>
//		static DEVICE deserialize(deserializtion_t data);

//		static DeviceIdentifier deserializeID(deserializtion_t data);
//};

//template<class SERIALIZER, class DEVICE>
//class DeviceSerializer
//{
//		template<class SERIALIZER_T = SERIALIZER, class DEVICE_T = DEVICE>
//		static constexpr void check()
//		{
//			    static_assert (DEVICE_SERIALIZER_METHODS_C<DeviceSerializerMethods<SERIALIZER>, SERIALIZER, DEVICE>,
//			        "SERIALIZER serialization type does not have a valid DeviceSerializerMethods struct available");
//		}

//	public:
//		using prop_deserialization_t = typename DeviceSerializerMethods<SERIALIZER>::prop_deserialization_t;
//		using deserialization_t = typename DeviceSerializerMethods<SERIALIZER>::deserialization_t;

//		template<class DEVICE_T>
//		requires(std::same_as<std::remove_cvref_t<DEVICE_T>, DEVICE>)
//		static SERIALIZER serialize(DEVICE_T &&dev)
//		{
//			check();
//			return DeviceSerializerMethods<SERIALIZER>::serialize(std::forward<DEVICE_T>(dev));
//		}

//		static SERIALIZER serializeID(const DeviceIdentifier &devID)
//		{
//			check();
//			return DeviceSerializerMethods<SERIALIZER>::serializeID(devID);
//		}

//		static DEVICE deserialize(DeviceIdentifier &&devID, deserialization_t data)
//		{
//			check();
//			return DeviceSerializerMethods<SERIALIZER>::template deserialize(std::move(devID), std::forward<deserialization_t>(data));
//		}

//		static DeviceIdentifier serializeID(deserialization_t data)
//		{
//			check();
//			return DeviceSerializerMethods<SERIALIZER>::serializeID(std::forward<deserialization_t>(data));
//		}
//};

#endif // DEVICE_SERIALIZER_H
