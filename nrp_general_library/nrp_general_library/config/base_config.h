#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include "nrp_general_library/utils/property_template.h"

#include <concepts>
#include <string>

/*!
 * \brief Base class for all configs. Each config type must have a unique name
 * \tparam FORMAT Format Type
 */
template<class FORMAT>
class BaseConfig
{
	public:
		BaseConfig(const std::string &configType)
		    : _configType(configType)
		{}

		virtual ~BaseConfig() = default;

		/*!
		 * \brief Get config type
		 */
		const std::string &configType() const
		{	return this->_configType;	}

		/*!
		 * \brief Write configuration as FORMAT type
		 * \return Returns configuration as FORMAT
		 */
		virtual FORMAT writeConfig() const = 0;

	private:
		std::string _configType;
};

template<class T, class FORMAT>
concept CONFIG_C = requires {
    { T::ConfigType };
    std::derived_from<T, BaseConfig<FORMAT> >;
};

//template<class CONFIG, class FORMAT, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
//class BaseConfigProperties
//        : public BaseConfig<FORMAT>,
//          public PropertyTemplate<CONFIG, PROP_NAMES, PROPERTIES...>
//{
//	public:
//		template<class ...T>
//		BaseConfigProperties(T &&...properties)
//		    : BaseConfig<FORMAT>(CONFIG::ConfigType),
//		      PropertyTemplate<CONFIG, PROP_NAMES, PROPERTIES...>(std::forward<T>(properties)...)
//		{	static_assert(CONFIG_C<CONFIG, FORMAT>, "Invalid CONFIG class");	}
//};


#endif // BASE_CONFIG_H
