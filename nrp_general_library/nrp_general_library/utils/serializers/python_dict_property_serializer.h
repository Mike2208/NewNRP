#ifndef PYTHON_DICT_PROPERTY_SERIALIZER_H
#define PYTHON_DICT_PROPERTY_SERIALIZER_H

#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <boost/python.hpp>

template<>
class ObjectPropertySerializerMethods<boost::python::dict>
        : public PropertySerializerGeneral
{
	public:
		using ObjectDeserializer = PropertySerializerGeneral::ObjectDeserializer<boost::python::dict>;

		template<class PROPERTY>
		static boost::python::object serializeSingleProperty(const PROPERTY &property)
		{
			return boost::python::object(property);
		}

		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(const boost::python::dict &data, const std::string_view &name)
		{
			try
			{
				return boost::python::extract<PROPERTY>(data[name.data()]);
			}
			catch(const boost::python::error_already_set &)
			{
				boost::python::handle_exception();
				throw std::out_of_range(std::string("Couldn't find dict element ") + name.data() + " while deserializing object");
			}
		}

		static SinglePropertyDeserializer<boost::python::object> deserializeSingleProperty(const boost::python::dict &data, const std::string_view &name)
		{
			return SinglePropertyDeserializer<boost::python::object>(data.get(name.data()));
		}

		static void emplaceSingleObject(boost::python::dict &data, const std::string_view &name, boost::python::object &&singleObject)
		{
			data[name.data()] = std::move(singleObject);
		}
};

/*!
 *	\brief python::dict object de/-serialization functions
 *	\tparam PROPERTY_TEMPLATE PropertyTemplate to de/-serialize
 */
template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
using PythonDictPropertySerializer = PropertySerializer<boost::python::dict, PROPERTY_TEMPLATE>;


#endif // PYTHON_DICT_PROPERTY_SERIALIZER_H
