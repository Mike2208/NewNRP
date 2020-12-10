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
//		using prop_deserialization_t = typename ObjectPropertySerializerMethods<SERIALIZER>::deserialization_t;
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

/*! \page device_serializers
DeviceSerializers contain static functions to de-/serialize \ref devices "Devices". They are all specializations of the DeviceSerializerMethods<> template class.
Each specialization must conform to a set of requirements, defined in the DEVICE_SERIALIZER_METHODS_C concept.

By providing a structured de-/serialization interface, other components of the NRP can use it to automatically integrate these methods into their workflow,
able to integrate new serialization methods on compile. See EngineDeviceController for an example.

The following example lists the requirements:
\code{.cpp}
// Device data will be de-/serialized from/to the SERIALIZER type
template<class SERIALIZER>
class DeviceSerializerMethods
{
	public:
		// prop_deserialization_t Defines the property deserialization type used by the PropertySerializer corresponding to the given SERIALIZER type
		// Usually, PROP_SERIALIZER is simply SERIALIZER
		using prop_deserialization_t = typename ObjectPropertySerializerMethods<PROP_SERIALIZER>::deserialization_t;

		// deserializtion_t Defines the deserialization type that will be converted to a DEVICE class. Usually this is just a const reference to the SERIALIZER type
		using deserializtion_t = const SERIALIZER&;

		// The serialize functions takes a DEVICE type and returns the data in serialized form
		template<DEVICE_C DEVICE>
		static SERIALIZER serialize(const DEVICE &dev);

		// The serializeID functions takes a DeviceIdentifier and generates the DeviceID in serialized form
		static SERIALIZER serializeID(const DeviceIdentifier &devID);

		// The deserialize functions takes data of the type deserializtion_t and constructs a given DEVICE type with it.
		template<DEVICE_C DEVICE>
		static DEVICE deserialize(deserializtion_t data);

		// The deserializeID functions takes data of the type deserializtion_t and extracts the DeviceIdentifier
		static DeviceIdentifier deserializeID(deserializtion_t data);
};
\endcode
*/

#endif // DEVICE_SERIALIZER_H
