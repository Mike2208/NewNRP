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
