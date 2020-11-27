#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/device_serializer.h"
#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/ptr_templates.h"
#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <concepts>
#include <string>
#include <string_view>
#include <memory>
#include <type_traits>

#include <nrp_grpc_library/engine_grpc.grpc.pb.h>

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
	 * \brief Corresponding engine
	 */
	std::string EngineName;

	/*!
	 * \brief Device Type
	 */
	std::string Type;

	DeviceIdentifier(const std::string &_name, const std::string &_engineName, const std::string &_type);

	template<class STRING1_T, class STRING2_T, class STRING3_T>
	requires(std::constructible_from<std::string, STRING1_T> && std::constructible_from<std::string, STRING2_T> && std::constructible_from<std::string, STRING3_T>)
	DeviceIdentifier(STRING1_T &&_name, STRING2_T &&_engineName, STRING3_T &&_type)
	    : Name(std::forward<STRING1_T>(_name)),
	      EngineName(std::forward<STRING2_T>(_engineName)),
	      Type(std::forward<STRING3_T>(_type))
	{}

	DeviceIdentifier() = default;

	bool operator== (const DeviceIdentifier &) const = default;
	auto operator<=>(const DeviceIdentifier &) const = default;
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
		DeviceInterface(const std::string &name, const std::string &engineName, const std::string &type);
		virtual ~DeviceInterface() = default;

		const std::string &name() const;
		void setName(const std::string &name);

		const std::string &type() const;
		void setType(const std::string &type);

		const std::string &engineName() const;
		void setEngineName(const std::string &engineName);

		const DeviceIdentifier &id() const;
		void setID(const DeviceIdentifier &id);

		virtual void serialize(EngineGrpc::SetDeviceMessage * request) const;
		virtual void deserialize(const EngineGrpc::GetDeviceMessage &deviceData);


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
        {	T::TypeName	};
        std::derived_from<T, DeviceInterface>;
        std::derived_from<T, PropertyTemplateGeneral>;
        std::constructible_from<T, DeviceIdentifier&&>;
        std::constructible_from<T, DeviceIdentifier&&, typename T::property_template_t &&>;
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
          public PropertyTemplate<DEVICE, PROP_NAMES, PROPERTIES...>,
          public PtrTemplates<DEVICE>
{
	public:
		static constexpr FixedString TypeName = TYPE;
		using property_template_t = typename PropertyTemplate<DEVICE, PROP_NAMES, PROPERTIES...>::property_template_t;

		using shared_ptr = typename PtrTemplates<DEVICE>::shared_ptr;
		using const_shared_ptr = typename PtrTemplates<DEVICE>::const_shared_ptr;
		using unique_ptr = typename PtrTemplates<DEVICE>::unique_ptr;
		using const_unique_ptr = typename PtrTemplates<DEVICE>::const_unique_ptr;

		virtual ~Device() override = default;

		/*!
		 * \brief Constructor
		 * \tparam DEV_ID_T DeviceIdentifier type
		 * \tparam PROPERTIES_T Property types to pass along to PropertyTemplate constructor
		 * \param devID Device ID
		 * \param props Properties to pass along to PropertyTemplate constructor
		 */
		template<class DEV_ID_T, class ...PROPERTIES_T>
		requires(std::same_as<std::remove_cvref_t<DEV_ID_T>, DeviceIdentifier>)
		Device(DEV_ID_T &&devID, PROPERTIES_T &&...props)
		    : DeviceInterface(std::forward<DEV_ID_T>(devID)),
		      property_template_t(std::forward<PROPERTIES_T>(props)...)
		{
			// Make sure DEVICE class is derived from DeviceInterface
			static_assert(DEVICE_C<DEVICE>, "DEVICE does not fulfill concept requirements");
		};

		static DeviceIdentifier createID(std::string &&name, std::string &&engineName)
		{	return DeviceIdentifier(std::move(name), std::move(engineName), DEVICE::TypeName);	}

		/*!
		 * \brief Deserialize data into new device
		 * \tparam DESERIALZER_T Type to deserialize
		 * \param id Device ID. Type must match device type
		 * \param data Data to deserialize
		 * \return Returns DEVICE
		 */
		template<class DESERIALIZER_T>
		static DEVICE deserialize(DeviceIdentifier &&id, DESERIALIZER_T &&data)
		{	return DeviceSerializer<std::remove_cvref_t<DESERIALIZER_T>, DEVICE>::template deserializer(std::move(id), std::forward<DESERIALIZER_T>(data));	}

		/*!
		 * \brief Update device by filling it with deserialized data
		 * \tparam PROP_DESERIALZER_T Type to deserialize
		 * \param data Data to deserialize
		 */
		template<class PROP_DESERIALIZER_T>
		void update(PROP_DESERIALIZER_T &&data)
		{	PropertySerializer<std::remove_cvref_t<PROP_DESERIALIZER_T>, DEVICE>::template updateProperties(*this, std::forward<PROP_DESERIALIZER_T>(data));	}

		/*!
		 * \brief Serialize device
		 */
		template<class SERIALIZER_T>
		SERIALIZER_T serialize()
		{	return DeviceSerializer<std::remove_cvref_t<SERIALIZER_T>, DEVICE>::template serialize(*this);	}

		/*!
		 * \brief Deserialize property data into PropertyTemplate format
		 * \tparam DESERIALZER_T Type to deserialize
		 * \tparam PROPERTIES_T Type of default properties
		 * \param data Property Data to deserialize
		 * \param props Default values. Used if data does not initialize a certain value
		 * \return Returns PropertyTemplate of this device
		 */
		template<class DESERIALIZER_T, class ...PROPERTIES_T>
		static typename DEVICE::property_template_t deserializeProperties(DESERIALIZER_T &&data, PROPERTIES_T &&...props)
		{
			return PropertySerializer<DESERIALIZER_T, DEVICE>::template readProperties(std::forward<DESERIALIZER_T>(data),
			                                                                           std::forward<PROPERTIES_T>(props)...);
		}
};


/*! \page devices Devices

Devices are data structures used for communicating with Engines. Their base class is DeviceInterface, which contains a DeviceIdentifier to identify the corresponding engine.
All devices must be de-/serializable in a manner that makes communication with an Engine possible. Usually, a PropertyTemplate can be used to Device data in a manner that is
easily serializable.

All currently documented Devices can be found \ref device "here".

 */

#endif // DEVICE_INTERFACE_H
