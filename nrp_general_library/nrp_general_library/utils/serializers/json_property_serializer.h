#ifndef JSON_PROPERTY_SERIALIZER_H
#define JSON_PROPERTY_SERIALIZER_H

#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <nlohmann/json.hpp>


/*!
 *	\brief De/-Serializer methods for JSON objects
 */
template<>
class ObjectPropertySerializerMethods<nlohmann::json>
        : public PropertySerializerGeneral
{
	public:
		using single_object_t = nlohmann::json;
		using ObjectDeserializer = typename PropertySerializerGeneral::ObjectDeserializer<nlohmann::json>;

		template<class PROPERTY>
		static nlohmann::json serializeSingleProperty(const PROPERTY &property)
		{
			return nlohmann::json(property);
		}

		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name)
		{
			const auto dataIterator(data.find(name.data()));
			if(dataIterator != data.end())
				return *dataIterator;
			else
				throw std::out_of_range(std::string("Couldn't find JSON attribute ") + name.data() + " during deserialization");
		}

		static SinglePropertyDeserializer<nlohmann::json> deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name)
		{
			return SinglePropertyDeserializer<nlohmann::json>(data[name.data()]);
		}

		static void emplaceSingleObject(nlohmann::json &data, const std::string_view &name, nlohmann::json &&singleObject)
		{
			data.emplace(name.data(), std::move(singleObject));
			//data[name.data()] = std::move(singleObject);
		}
};

/*!
 *	\brief JSON object de/-serialization functions
 *	\tparam PROPERTY_TEMPLATE PropertyTemplate to de/-serialize
 */
template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
using JSONPropertySerializer = PropertySerializer<nlohmann::json, PROPERTY_TEMPLATE>;

/*!
 *	\brief PropertyTemplate with JSON object de/-serialization functions
 *	\tparam CLASS Final class derived from JSONPropertySerializerTemplate
 *	\tparam PROP_NAMES Template class of type PropNames<...>, containing all property names
 *	\tparam PROPERTIES Property classes
 */
template<class CLASS, class PROP_NAMES = PropNames<>, class ...PROPERTIES>
using JSONPropertySerializerTemplate = PropertySerializerTemplate<nlohmann::json, CLASS, PROP_NAMES, PROPERTIES...>;

#endif // JSON_PROPERTY_SERIALIZER_H
