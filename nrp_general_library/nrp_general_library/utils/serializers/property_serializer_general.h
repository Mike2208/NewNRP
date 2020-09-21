#ifndef PROPERTY_SERIALIZER_GENERAL_H
#define PROPERTY_SERIALIZER_GENERAL_H

#include "nrp_general_library/utils/property_template.h"

template<class OBJECT>
class ObjectPropertySerializerMethods;

class ObjectDeserializerGeneral
{};

/*!
 * \brief De-/Serialization class for PropertyTemplates.
 * Contains helper functions for PropertySerializer to use. To create de-/serializtion methods for a new OBJECT type,
 * implement a new ObjectPropertySerializerMethods<OBJECT>. The methods can then be accessed using PropertySerializer<OBJECT>
 */
class PropertySerializerGeneral
{
	protected:
		/*!
		 * \brief Class that will be passed to PropertyTemplate constructor for deserialization.
		 * Stores a reference to data to be deserialized during construction
		 */
		template<class OBJECT>
		class ObjectDeserializer
		        : public ObjectDeserializerGeneral
		{
			public:
				ObjectDeserializer(const OBJECT &data)
				    : _data(data)
				{}

				template<class PROPERTY>
				PROPERTY deserializeSingleProperty(const std::string_view &name)
				{	return ObjectPropertySerializerMethods<OBJECT>::template deserializeSingleProperty<PROPERTY>(this->_data, name);	}

			private:
				const OBJECT &_data;
		};


	public:
		/*!
		 * \brief Serialization function. Calls internal method that iterates over all properties
		 * \tparam OBJECT Type to serialize PropertyTemplate to
		 * \tparam PROPERTY_TEMPLATE PropertyTemplate<...> to serialize
		 * \tparam PROPERTY_TEMPLATE_T PropertyTemplate<...> to serialize. Used for std::forward
		 * \tparam OBJECT_T Type to serialize to. Used for std::forward
		 * \param Properties Properties to serialize
		 * \param data Serialization target
		 * \return Returns serialized data
		 */
		template<class OBJECT, class PROPERTY_TEMPLATE, class PROPERTY_TEMPLATE_T, class OBJECT_T>
		static OBJECT serializeObject(PROPERTY_TEMPLATE_T &&properties, OBJECT_T &&data = OBJECT())
		{	return PropertySerializerGeneral::serializeSingleProperty<OBJECT, PROPERTY_TEMPLATE, 0>(std::forward<OBJECT_T>(data), std::forward<PROPERTY_TEMPLATE_T>(properties));	}

		/*!
		 * \brief Update a PropertyTemplate with new data
		 * \tparam OBJECT Deserialization type
		 * \tparam PROPERTY_TEMPLATE PropertyTemplate<...>
		 * \tparam OBJECT_T Deserialization type. Used for std::forward
		 * \param properties PropertyTemplate to update
		 * \param data Data with which to update properties
		 */
		template<class OBJECT, class PROPERTY_TEMPLATE, class OBJECT_T>
		static void updateProperties(PROPERTY_TEMPLATE &properties, OBJECT_T &&data)
		{	return PropertySerializerGeneral::updateProperties<OBJECT, PROPERTY_TEMPLATE, 0>(properties, std::forward<OBJECT_T>(data));	}

		/*!
		 * \brief Read out data from an OBJECT class into a PROPERTY_TEMPLATE class
		 * \tparam OBJECT Deserialization type
		 * \tparam PROPERTY_TEMPLATE PropertyTemplate<...>
		 * \tparam OBJECT_T Deserialization type. Used for std::forward
		 * \tparam DEFAULT_PROPERTIES_T Type for default Properties
		 * \param data Data to read
		 * \param defaultProperties Default Properties to use if no data could be read to a given Property
		 * \return Returns a PROPERTY_TEMPLATE instance
		 */
		template<class OBJECT, class PROPERTY_TEMPLATE, class OBJECT_T, class ...DEFAULT_PROPERTIES_T>
		static PROPERTY_TEMPLATE deserializeObject(OBJECT_T &&data, DEFAULT_PROPERTIES_T &&...defaultProperties)
		{	return PROPERTY_TEMPLATE(typename ObjectPropertySerializerMethods<OBJECT>::ObjectDeserializer(std::forward<OBJECT_T>(data)), std::forward<DEFAULT_PROPERTIES_T>(defaultProperties)...);	}

	protected:

		/*!
		 * \brief Serialize a single property. If PROPERTY is a PropertyTemplate, serialize it entirely
		 * \tparam OBJECT Type to serialize PropertyTemplate to
		 * \tparam PROPERTY Property type to serialize
		 * \param property Property to serialize
		 * \return Returns serialized data of a single object.
		 * Must be in a type that ObjectPropertySerializerMethods<OBJECT>::emplaceSingleObject() can handle
		 */
		template<class OBJECT, class PROPERTY, class PROPERTY_T>
		static auto serializeSingleProperty(PROPERTY_T &&property)
		{
			if constexpr (std::is_base_of_v<PropertyTemplateGeneral, PROPERTY>)
			{
				// If this is Property is itself a PropertyTemplate, run serialization on it
				return PropertySerializerGeneral::serializeObject<OBJECT, PROPERTY>(std::forward<PROPERTY_T>(property), OBJECT());
			}
			else
			{
				// Call Property Serializer associated with this object
				//static_assert (std::is_invocable_v<decltype(ObjectPropertySerializerMethods<OBJECT>::template serializeSingleProperty<PROPERTY>), decltype((property))>, "No ObjectPropertySerializerMethods found for this object type");
				return ObjectPropertySerializerMethods<OBJECT>::serializeSingleProperty(std::forward<PROPERTY_T>(property));
			}
		}

	private:
		/*!
		 * \brief Write a single property. Continue until end of PropertyTemplate has been reached
		 * \tparam OBJECT Type to serialize PropertyTemplate to
		 * \tparam PROPERTY_TEMPLATE PropertyTemplate<...> to serialize
		 * \tparam ID ID of property to write
		 * \param data Data structure to write to
		 * \param properties PropertyTemplate from which to read data
		 * \return Returns data, with property appended to it
		 */
		template<class OBJECT, class PROPERTY_TEMPLATE, int ID, class PROPERTY_TEMPLATE_T>
		static OBJECT serializeSingleProperty(OBJECT &&data, PROPERTY_TEMPLATE_T &&properties)
		{
			using property_t = typename PROPERTY_TEMPLATE::template property_t<ID>;
			if constexpr (ID < PROPERTY_TEMPLATE::NumProperties)
			{
				// Check if default value should be serialized
				if constexpr (PROPERTY_TEMPLATE::template isDefaultWritable<ID>())
				{
					ObjectPropertySerializerMethods<OBJECT>::emplaceSingleObject(data, properties.template getName<ID>(), PropertySerializerGeneral::serializeSingleProperty<OBJECT, typename PROPERTY_TEMPLATE::template property_t<ID> >(properties.template getProperty<ID>()));
				}
				else
				{
					// Only write this property if it does not correspond to the default
					const auto &defValue = properties.template getDefaultValue<ID>();
					const auto &value = properties.template getProperty<ID, property_t>();
					if(value != defValue)
						ObjectPropertySerializerMethods<OBJECT>::emplaceSingleObject(data, properties.template getName<ID>(), PropertySerializerGeneral::serializeSingleProperty<OBJECT, typename PROPERTY_TEMPLATE::template property_t<ID> >(value));
				}

				// Continue with next property
				return PropertySerializerGeneral::template serializeSingleProperty<OBJECT, PROPERTY_TEMPLATE, ID+1>(std::move(data), std::forward<PROPERTY_TEMPLATE_T>(properties));
			}
			else
			{
				// Stop at end of PropertyTemplate
				return std::move(data);
				//return data;
			}
		}

		/*!
		 * \brief Update a given PropertyTemplate
		 * \tparam OBJECT Deserialization type
		 * \tparam PROPERTY_TEMPLATE PropertyTemplate<...>
		 * \tparam OBJECT_T Deserialization type. Used for std::forward
		 * \param properties Property Structure to update
		 * \param data Data to read from
		 */
		template<class OBJECT, class PROPERTY_TEMPLATE, int ID, class OBJECT_T>
		static void updateProperties(PROPERTY_TEMPLATE &properties, OBJECT_T &&data)
		{
			// Continue until end of Properties reached
			if constexpr (ID < PROPERTY_TEMPLATE::NumProperties)
			{
				// Try to update. If no update available, continue
				try
				{
					using property_t = typename PROPERTY_TEMPLATE::template property_t<ID>;
					properties.template getProperty<ID, property_t>() = ObjectPropertySerializerMethods<OBJECT>::template deserializeSingleProperty<property_t>(std::forward<OBJECT_T>(data), properties.template getName<ID>());
				}
				catch(const std::exception &)
				{
					// TODO: Create specific exception to throw on deserialization failure
				}

				// Return Updated properties
				return PropertySerializerGeneral::template updateProperties<OBJECT, PROPERTY_TEMPLATE, ID+1, OBJECT_T>(properties, std::forward<OBJECT_T>(data));
			}
		}
};

#endif // PROPERTY_SERIALIZER_GENERAL_H
