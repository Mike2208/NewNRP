#ifndef JSON_CONFIG_PROPERTIES_H
#define JSON_CONFIG_PROPERTIES_H

#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/config/base_config.h"

#include <string_view>
#include <nlohmann/json.hpp>

/*!
 *	\brief Reads and writes configuration from and to JSON format
 *	\tparam CONFIG Final class derived from this JSONConfigProperties
 *	\tparam PROP_NAMES Class of type PropNames<...> containing all Property names
 *	\tparam PROPERTIES All property types that should be present in JSON
 */
template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class JSONConfigProperties
        : public BaseConfig<nlohmann::json>,
          public PropertyTemplate<CONFIG, PROP_NAMES, PROPERTIES...>
{
		using property_template_t = PropertyTemplate<CONFIG, PROP_NAMES, PROPERTIES...>;
		using json_property_serializer_t = JSONPropertySerializer<property_template_t>;

	public:
		/*!
		 * \brief Constructor. Reads properties from given JSON object.
		 * \tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 * \param config JSON object containing property data
		 * \param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<class ...T>
		JSONConfigProperties(const nlohmann::json &config, T&&... defaultProperties)
		    : BaseConfig(CONFIG::ConfigType),
		      property_template_t(JSONConfigProperties::readConfig(config, std::forward<T>(defaultProperties)...))
		{	static_assert(CONFIG_C<CONFIG, nlohmann::json>, "Invalid CONFIG class");	}

		/*!
		 *	\brief Update configuration by reading the given JSON object
		 *	\tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 *	\param config JSON object containing property data
		 *	\param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<class ...T>
		void updateConfig(const nlohmann::json &config)
		{	json_property_serializer_t::updateProperties(static_cast<property_template_t&>(*this), config);	}

		/*!
		 *	\brief Read configuration from the given JSON object
		 *	\tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 *	\param config JSON object containing property data
		 *	\param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<class ...T>
		static property_template_t readConfig(const nlohmann::json &config, T &&...defaultProperties)
		{	return json_property_serializer_t::readProperties(config, std::forward<T>(defaultProperties)...);	}

		/*!
		 * \brief Converts configProperties into a JSON object
		 * \param configProperties Properties to convert
		 * \return Returns a JSON object. For each property, the value will be stored under its given name
		 */
		static nlohmann::json writeConfig(const property_template_t &configProperties)
		{	return json_property_serializer_t::serializeProperties(configProperties, nlohmann::json());	}

		nlohmann::json writeConfig() const override
		{	return JSONConfigProperties::writeConfig(static_cast<const property_template_t&>(*this));	}
};

#endif //JSON_CONFIG_PROPERTIES_H
