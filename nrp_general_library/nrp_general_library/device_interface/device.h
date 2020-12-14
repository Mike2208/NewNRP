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

#ifndef DEVICE_H
#define DEVICE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/device_interface/device_serializer.h"

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

		template<class STRING1_T, class STRING2_T>
		requires(std::constructible_from<std::string, STRING1_T> && std::constructible_from<std::string, STRING2_T>)
		static DeviceIdentifier createID(STRING1_T &&name, STRING2_T &&engineName)
		{	return DeviceIdentifier(std::forward<STRING1_T>(name), std::forward<STRING2_T>(engineName), DEVICE::TypeName);	}

//		/*!
//		 * \brief Deserialize data into new device
//		 * \tparam DESERIALZER_T Type to deserialize
//		 * \param id Device ID. Type must match device type
//		 * \param data Data to deserialize
//		 * \return Returns DEVICE
//		 */
//		template<class DESERIALIZER_T, class ...PROPS_T>
//		static DEVICE deserialize(DeviceIdentifier &&id, DESERIALIZER_T &&data)
//		{	return DeviceSerializer<std::remove_cvref_t<DESERIALIZER_T>, DEVICE>::template deserialize(std::move(id), std::forward<DESERIALIZER_T>(data));	}

//		/*!
//		 * \brief Update device by filling it with deserialized data
//		 * \tparam PROP_DESERIALZER_T Type to deserialize
//		 * \param data Data to deserialize
//		 */
//		template<class PROP_DESERIALIZER_T>
//		void update(PROP_DESERIALIZER_T &&data)
//		{	PropertySerializer<std::remove_cvref_t<PROP_DESERIALIZER_T>, DEVICE>::template updateProperties(*this, std::forward<PROP_DESERIALIZER_T>(data));	}

//		/*!
//		 * \brief Serialize device
//		 */
//		template<class SERIALIZER_T>
//		SERIALIZER_T serialize() const
//		{	return DeviceSerializer<std::remove_cvref_t<SERIALIZER_T>, DEVICE>::template serialize(*this);	}

		/*!
		 * \brief Deserialize property data into PropertyTemplate format
		 * \tparam DESERIALZER_T Type to deserialize
		 * \tparam PROPERTIES_T Type of default properties
		 * \param data Property Data to deserialize
		 * \param props Default values. Used if data does not initialize a certain value
		 * \return Returns PropertyTemplate of this device
		 */
		template<class DESERIALIZER_T, class ...PROPERTIES_T>
		static property_template_t deserializeProperties(DESERIALIZER_T &&data, PROPERTIES_T &&...props)
		{
			return PropertySerializer<std::remove_cvref_t<DESERIALIZER_T>, DEVICE>::template readProperties(std::forward<DESERIALIZER_T>(data),
			                                                                                                std::forward<PROPERTIES_T>(props)...);
		}
};


/*! \page devices Devices

\copydoc device_usage_section

The Device base class is DeviceInterface, which contains the DeviceIdentifier necessary for identifying the corresponding engine. All devices must conform to the DEVICE_C concept.
Additionally, they must be de-/serializable in a manner that makes communication with an Engine possible. See \ref device_serializer for details on implementing de-/serialization.

To create new devices, developers can use the Device template as a base. It derives a DeviceInterface as well as a PropertyTemplate containing all requested properties.

An example device class would be:
\code{.cpp}
class NewDevice
	: public Device<NewDevice, "NewDevice", PropNames<"intValue", "stringValue">, int, std::string>
{
	public:
		// Constructor. property_template_t refers to the PropertyTemplate<> base class.
		// Note the default property values. If no property_template_t is specified, they will be used
		NewDevice(DeviceIdentifier &&devID, property_template_t props = property_template_t(5, "empty")
			: Device(std::move(devID), std::move(props))
		{}

		// Property Deserializer. The base Device::deserializeProperties references the DeviceSerializerMethods template specialized by DESERIALIZER_T
		// Note the default property values. If no property_template_t is specified, they will be used. They should be the same as the ones in the constructor above
		// Also note that this function override must only be provided if default property values should be set on instantiation
		template<class DESERIALIZE_T>
		static deserializeProperties(DESERIALIZE_T &&data)
		{	return Device::deserializeProperties(std::forward<DESERIALIZE_T>(data), 5, "empty");	}

		// Add functions to access property values easier
		constexpr const int &intVal() const
		{	return this->getPropertyByName<"intValue">();	}
		constexpr int &intVal()
		{	return this->getPropertyByName<"intValue">();	}

		constexpr const std::string &strVal() const
		{	return this->getPropertyByName<"stringValue">();	}
		constexpr std::string &strVal()
		{	return this->getPropertyByName<"stringValue">();	}
}
\endcode

 */

#endif // DEVICE_H
