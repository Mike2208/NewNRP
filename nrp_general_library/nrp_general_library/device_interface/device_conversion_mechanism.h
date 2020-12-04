#ifndef DEVICE_CONVERSION_MECHANISM_H
#define DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"

#include <concepts>

/*!
 *	\brief Concept for Device Conversion Mechanism Types.
 *	Ensures that the class can call:
 *	  - void init(const EngineInterface &)
 *	  - void shutdown()
 *    - const SERIALIZATION_TYPE &serialize(DEVICE &device)
 *    - const DEVICE &deserialize(DESERIALIZATION_TYPE &data)
 *    - const DeviceIdentifier &getID(const DESERIALIZATION_TYPE &data)
 *  \tparam SERIALIZATION_TYPE Device Serialization type
 *	\tparam DESERIALIZATION_TYPE Device Deserialization type, usually same as SERIALIZATION_TYPE
 *	\tparam DEVICES Devices that should be converted
 */
template<class T, class SERIALIZATION_TYPE, class DESERIALIZATION_TYPE, class ...DEVICES>
concept DEVICE_CONVERSION_MECHANISM_C =
        (DEVICE_C<DEVICES> && ...) &&
        (std::is_invocable_r_v<const SERIALIZATION_TYPE &, decltype(std::declval<T>().serialize), DEVICES&> && ...) &&
        (std::is_invocable_r_v<const DEVICES&, decltype(std::declval<T>().deserialize), DESERIALIZATION_TYPE &> && ...) &&
        std::is_invocable_r_v<const DeviceIdentifier &, std::declval<T>().getID, const DESERIALIZATION_TYPE &> &&
        std::is_invocable_v<std::declval<T>().init, const EngineInterface &> &&
        std::is_invocable_v<std::declval<T>().shutdown>;

/*!
 *	\brief Example DeviceConversionMechanism
 */
template<class SERIALIZATION_TYPE, class DESERIALIZATION_TYPE = SERIALIZATION_TYPE, DEVICE_C ...DEVICES>
class DeviceConversionMechanism
{
		template<DEVICE_C DEVICE, DEVICE_C ...REM>
		struct Conversions
		        : public Conversions<REM...>
		{
			static const SERIALIZATION_TYPE &serialize(DEVICE &device);
			static const DEVICE &deserialize(DESERIALIZATION_TYPE &data);
		};

	public:
		/*!
		 * \brief DCM Initialization
		 * \param engine Engine related to conversion routine
		 */
		static void init(const EngineInterface &engine);

		/*!
		 * \brief DCM Shutdown
		 */
		static void shutdown();

		/*!
		 * \brief Device Serialization
		 * \tparam DEVICE Device Type
		 * \param device Device to serialize
		 * \return Returns serialized data
		 */
		template<DEVICE_C DEVICE>
		static const SERIALIZATION_TYPE &serialize(DEVICE &device)
		{	return Conversions<DEVICES...>::serialize(device);	}

		/*!
		 * \brief Device Deserialization
		 * \tparam DEVICE Device Type
		 * \param data Serialized data
		 * \return Returns deserialized device
		 */
		template<DEVICE_C DEVICE>
		static const DEVICE &deserialize(DESERIALIZATION_TYPE &data)
		{	return Conversions<DEVICES...>::deserialize(data);	}

		/*!
		 * \brief Extract ID from serialized data
		 * \param data Serialized Data
		 * \return Returns Device ID
		 */
		static const DeviceIdentifier &getID(const DESERIALIZATION_TYPE &data);
};

#endif	// DEVICE_CONVERSION_MECHANISM_H
