#ifndef JSON_DEVICE_INTERFACE_H
#define JSON_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

/*!
 * \brief General Class for all JSONDevices
 */
class JSONDeviceInterfaceGeneral
{};

/*!
 *	\brief Concept of classes derived from JSONDeviceInterface.
 *	Also ensures that devices are constructible via T(const DeviceIdentifier&, const nlohmann::json&)
 */
template<class T>
concept JSON_DEVICE_C =
    std::is_base_of_v<DeviceInterface, T> &&
    std::is_constructible_v<T, const DeviceIdentifier&, const nlohmann::json&>;

/*!
 *	\brief Device Interface with JSON Serializer
 */
template<class DEVICE, PROP_NAMES_C PROP_NAMES = PropNames<>, class ...PROPERTIES>
class JSONDeviceInterface
        : public DeviceInterface,
          public JSONDeviceInterfaceGeneral,
          public JSONPropertySerializerTemplate<DEVICE, PROP_NAMES, PROPERTIES...>
{
		using json_property_serializer_t = JSONPropertySerializerTemplate<DEVICE, PROP_NAMES, PROPERTIES...>;

	protected:
		using json_device_interface_t = JSONDeviceInterface<DEVICE, PROP_NAMES, PROPERTIES...>;

	public:
		virtual ~JSONDeviceInterface() = default;

		template<class ...PROPERTIES_T>
		JSONDeviceInterface(const DeviceIdentifier &devID, const nlohmann::json &data, PROPERTIES_T &&...properties)
		    : DeviceInterface(devID),
		      json_property_serializer_t(data, std::forward<PROPERTIES_T>(properties)...)
		{}

		template<class ...PROPERTIES_T>
		JSONDeviceInterface(const DeviceIdentifier &devID, PROPERTIES_T &&...properties)
		    : DeviceInterface(devID),
		      json_property_serializer_t(std::forward<PROPERTIES_T>(properties)...)
		{}
};

#endif // JSON_DEVICE_INTERFACE_H
