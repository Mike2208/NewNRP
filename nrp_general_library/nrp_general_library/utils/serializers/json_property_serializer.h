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

#ifndef JSON_PROPERTY_SERIALIZER_H
#define JSON_PROPERTY_SERIALIZER_H

#include "nrp_general_library/utils/nrp_exceptions.h"
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
		using ObjectDeserializer = typename PropertySerializerGeneral::ObjectDeserializer<nlohmann::json>;
		using deserialization_t = const nlohmann::json&;

		template<class PROPERTY>
		static nlohmann::json serializeSingleProperty(const PROPERTY &property)
		{	return nlohmann::json(property);	}

		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(const nlohmann::json &data, const std::string_view &name)
		{
			const auto dataIterator(data.find(name.data()));
			if(dataIterator != data.end())
				return *dataIterator;
			else
				throw NRPExceptionMissingProperty(std::string("Couldn't find JSON attribute \"") + name.data() + "\" during deserialization");
		}

		static void emplaceSingleObject(nlohmann::json &data, const std::string_view &name, nlohmann::json &&singleObject)
		{
			data.emplace(name.data(), std::move(singleObject));
			//data[name.data()] = std::move(singleObject);
		}
};

using JSONPropertySerializerMethods = ObjectPropertySerializerMethods<nlohmann::json>;

/*!
 *	\brief JSON object de/-serialization functions
 *	\tparam PROPERTY_TEMPLATE PropertyTemplate to de/-serialize
 */
template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
using JSONPropertySerializer = PropertySerializer<nlohmann::json, PROPERTY_TEMPLATE>;

#endif // JSON_PROPERTY_SERIALIZER_H
