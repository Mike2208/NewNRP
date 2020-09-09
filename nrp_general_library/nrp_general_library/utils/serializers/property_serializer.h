#ifndef PROPERTY_SERIALIZER_H
#define PROPERTY_SERIALIZER_H

#include "nrp_general_library/utils/serializers/property_serializer_general.h"

/*!
 * \brief Class for de-/serialization of data. Must be further specified for specific OBJECT types
 * \brief OBJECT Type to de-/serialize
 */
template<class OBJECT>
class ObjectPropertySerializerMethods
        : public PropertySerializerGeneral
{
	public:
		using single_object_t = int;

		/*!
		 *	\brief Serialize a single property
		 *	\tparam PROPERTY Property Type
		 *	\param property Property to serialize
		 *	\return Returns serialized type
		 */
		template<class PROPERTY>
		static single_object_t serializeSingleProperty(const PROPERTY &property);

		/*!
		 * \brief Add a serialized property to an object
		 * \param data All serialized data
		 * \param name Name under which to store singleObject
		 * \param singleObject Serialized Proerty data
		 */
		static void emplaceSingleObject(OBJECT &data, const std::string_view &name, single_object_t &&singleObject);

		/*!
		 * \brief Deserialize a single property
		 * \tparam Deserialized property type
		 * \param data All serialized data
		 * \param name Name under which the property to deserialize is stored
		 * \return Returns deserialized property
		 */
		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(const OBJECT &data, const std::string_view &name);

		/*!
		 * \brief Deserialize a single property
		 * \param data All serialized data
		 * \param name Name under which the property to deserialize is stored
		 * \return Returns property deserializer. Can be used to convert the property to a chosen type later on
		 */
		static SinglePropertyDeserializer<single_object_t> deserializeSingleProperty(const OBJECT &data, const std::string_view &name);
};

template<class T, class OBJECT>
concept PROPERTY_SERIALIZER_OBJECT_C = std::is_same_v<OBJECT, std::remove_cv_t<std::decay_t<T> > >;

/*!
 *	\brief De-/Serialization Methods. This class collects ObjectPropertySerializerMethods
 */
template<class OBJECT, PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
struct PropertySerializer
{
		/*!
		 *	\brief Update Properties by reading the given OBJECT. Will go through all existing properties and try to update them
		 *	\tparam OBJECT Data type to be deserialized
		 *	\param data OBJECT containing property data
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T>
		static void readProperties(PROPERTY_TEMPLATE &properties, OBJECT_T &&data)
		{	PropertySerializerGeneral::template updateProperties<OBJECT, PROPERTY_TEMPLATE, OBJECT_T>(properties, std::forward<OBJECT_T>(data));	}

		/*!
		 *	\brief Read properties from the given OBJECT
		 *	\tparam OBJECT Data type to be deserialized
		 *	\tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 *	\param data OBJECT containing property data
		 *	\param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T, class ...T>
		static PROPERTY_TEMPLATE readProperties(OBJECT_T &&data, T &&... defaultProperties)
		{	return PropertySerializerGeneral::template deserializeObject<OBJECT, PROPERTY_TEMPLATE, OBJECT_T, T...>(std::forward<OBJECT_T>(data), std::forward<T>(defaultProperties)...);	}

		/*!
		 *	\brief Serializes properties into OBJECT
		 *	\tparam OBJECT Data type to be serialized
		 *	\param properties Properties to convert
		 *	\param data OBJECT into which to insert the serialized data
		 *	\return Returns an OBJECT. For each property, the value will be stored under its given name
		 */
		static OBJECT serializeProperties(const PROPERTY_TEMPLATE &properties, OBJECT &&data = OBJECT())
		{	return PropertySerializerGeneral::template serializeObject<OBJECT, PROPERTY_TEMPLATE>(properties, std::move(data));	}
};

/*!
 *	\brief Serializes PropertyTemplate
 *	\tparam OBJECT Serialization type
 *	\tparam CLASS Final class derived from PropertySerializerTemplate
 *	\tparam PROP_NAMES Property Names. PropNames<...> type
 *	\tparam PROPERTIES Property classes
 */
template<class OBJECT, class CLASS, PROP_NAMES_C PROP_NAMES = PropNames<>, class ...PROPERTIES>
class PropertySerializerTemplate
        : public PropertyTemplate<CLASS, PROP_NAMES, PROPERTIES...>
{
	public:
		using property_template_t = PropertyTemplate<CLASS, PROP_NAMES, PROPERTIES...>;
		using property_serializer_t = PropertySerializer<OBJECT, property_template_t >;

		/*!
		 * \brief Constructor. Reads data
		 * \tparam OBJECT_T Object type to deserialize
		 * \tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 * \param data Object to deserialize
		 * \param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T, class ...T>
		PropertySerializerTemplate(OBJECT_T &&data, T &&...defaultProperties)
		    : property_template_t(property_serializer_t::readProperties(std::forward<OBJECT_T>(data), std::forward<T>(defaultProperties)...))
		{}

		/*!
		 * \brief Constructor. Passes properties onto PropertyTemplate
		 * \tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 * \param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<class ...T>
		PropertySerializerTemplate(T &&...defaultProperties)
		    : property_template_t(std::forward<T>(defaultProperties)...)
		{}

		/*!
		 *	\brief Update Properties by reading the given OBJECT
		 *	\tparam OBJECT Data type to be deserialized
		 *	\tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 *	\param data OBJECT containing property data
		 *	\param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T, class ...T>
		void updateProperties(OBJECT_T &&data, T &&...defaultProperties)
		{	*this = PropertySerializerGeneral::template deserializeObject<OBJECT, property_template_t, OBJECT_T, T...>(std::forward<OBJECT_T>(data), std::forward<T>(defaultProperties)...);	}

		/*!
		 *	\brief Update Properties by reading the given OBJECT. Will go through all existing properties and try to update them
		 *	\tparam OBJECT Data type to be deserialized
		 *	\param data OBJECT containing property data
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T>
		void updateProperties(OBJECT_T &&data)
		{	PropertySerializerGeneral::template updateProperties<OBJECT, property_template_t, OBJECT_T>(*this, std::forward<OBJECT_T>(data));	}

		/*!
		 *	\brief Converts stored PROPERTIES into an OBJECT
		 *	\tparam OBJECT Data type to be serialized
		 *	\param data OBJECT into which to insert the serialized data
		 *	\return Returns an OBJECT. For each property, the value will be stored under its given name
		 */
		OBJECT serializeProperties(OBJECT &&data) const
		{	return PropertySerializerGeneral::template serializeObject<OBJECT, property_template_t>(*this, std::move(data));	}

		/*!
		 *	\brief Update Properties by reading the given OBJECT. Will go through all existing properties and try to update them
		 *	\tparam OBJECT Data type to be deserialized
		 *	\param data OBJECT containing property data
		 *	\param properties Properties data that should be updated. For each property, data is checked for the property name and, if found, updated.
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T>
		static void readProperties(PropertyTemplate<CLASS, PROP_NAMES, PROPERTIES...> &properties, OBJECT_T &&data)
		{	property_serializer_t::readProperties(properties, std::forward<OBJECT_T>(data));	}

		/*!
		 *	\brief Read properties from the given OBJECT
		 *	\tparam OBJECT Data type to be deserialized
		 *	\tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 *	\param data OBJECT containing property data
		 *	\param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<OBJECT> OBJECT_T, class ...T>
		static PropertyTemplate<CLASS, PROP_NAMES, PROPERTIES...> readProperties(OBJECT_T &&data, T &&...defaultProperties)
		{	return property_serializer_t::readProperties(std::forward<OBJECT_T>(data), std::forward<T>(defaultProperties)...);	}

		/*!
		 *	\brief Serializes properties into OBJECT
		 *	\tparam OBJECT Data type to be serialized
		 *	\param properties Properties to convert
		 *	\param data OBJECT into which to insert the serialized data
		 *	\return Returns an OBJECT. For each property, the value will be stored under its given name
		 */
		static OBJECT serializeProperties(const PropertyTemplate<CLASS, PROP_NAMES, PROPERTIES...> &properties, OBJECT &&data = OBJECT())
		{	return property_serializer_t::serializeProperties(properties, std::move(data));	}
};

#endif // PROPERTY_SERIALIZER_H
