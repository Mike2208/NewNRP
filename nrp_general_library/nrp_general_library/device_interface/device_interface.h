#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include <concepts>
#include <string>
#include <string_view>
#include <memory>
#include <type_traits>

/*!
 * \brief Identifies a single device
 */
struct DeviceIdentifier
{
	/*!
	 * \brief Device Name. Used by simulator to identify source/sink of device
	 */
	std::string Name;

	/*!
	 * \brief Device Type
	 */
	std::string Type;

	/*!
	 * \brief Corresponding engine
	 */
	std::string EngineName;

	DeviceIdentifier(const std::string &_name, const std::string &_type, const std::string &_engineName);
	DeviceIdentifier() = default;

	bool operator==(const DeviceIdentifier &other) const;
	bool operator<(const DeviceIdentifier &other) const;
};

/*!
 * \brief Interface to devices, both for physics as well as brain simulators
 */
class DeviceInterface
        : public PtrTemplates<DeviceInterface>
{
	public:
		enum RESULT
		{ SUCCESS, ERROR };

		DeviceInterface() = default;
		DeviceInterface(const DeviceIdentifier &id);
		DeviceInterface(const std::string &name, const std::string &type, const std::string &engineName);
		virtual ~DeviceInterface() = default;

		const std::string &name() const;
		void setName(const std::string &name);

		const std::string &type() const;
		void setType(const std::string &type);

		const std::string &engineName() const;
		void setEngineName(const std::string &engineName);

		const DeviceIdentifier &id() const;
		void setID(const DeviceIdentifier &id);

	private:
		/*!
		 * \brief Identifies Device. Contains name and type of this device
		 */
		DeviceIdentifier _id;
};

using DeviceInterfaceSharedPtr      = DeviceInterface::shared_ptr;
using DeviceInterfaceConstSharedPtr = DeviceInterface::const_shared_ptr;

template<class T>
concept DEVICE_C = requires {
        std::derived_from<T, DeviceInterface>;
        {	T::TypeName	};
};

/*!
 * \brief Device class. All devices must inherit from this one
 * \tparam DEVICE Final derived device class
 * \tparam TYPE Device Type
 * \tparam PROP_NAMES Property Names
 * \tparam PROPERTIES Device Properties
 */
template<class DEVICE, FixedString TYPE, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class Device
        : public DeviceInterface,
          public PropertyTemplate<DEVICE, PROP_NAMES, PROPERTIES...>
{
	public:
		static constexpr FixedString TypeName = TYPE;
		using property_template_t = typename PropertyTemplate<DEVICE, PROP_NAMES, PROPERTIES...>::property_template_t;

		virtual ~Device() override = default;

		/*!
		 * \brief Constructor
		 * \tparam PROPERTIES_T Property types to pass along to PropertyTemplate constructor
		 * \param devID Device ID
		 * \param props Properties to pass along to PropertyTemplate constructor
		 */
		template<class ...PROPERTIES_T>
		Device(const DeviceIdentifier &devID, PROPERTIES_T &&...props)
		    : DeviceInterface(devID),
		      property_template_t(std::forward<PROPERTIES_T>(props)...)
		{
			// Make sure DEVICE class is derived from DeviceInterface
			static_assert(DEVICE_C<DEVICE>, "DEVICE does not fulfill concept requirements");
		};
};

/*! \page devices Devices

Devices are data structures used for communicating with Engines. Their base class is DeviceInterface, which contains a DeviceIdentifier to identify the corresponding engine.
All devices must be de-/serializable in a manner that makes communication with an Engine possible. Usually, a PropertyTemplate can be used to Device data in a manner that is
easily serializable.

All currently documented Devices can be found \ref device "here".

 */

#endif // DEVICE_INTERFACE_H
