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

/*! \page config
Configuration files are based on \ref property_template "PropertyTemplates". In particular, they are used as basis for deserializing information out of JSON files,
and making them available to the program.

To create a new configuration file class, derive an instance of the JSONConfigProperties template. Note that engine configuration files are slightly different.
As all Engines share a number of parameters, a base template is provided to derive new engine configurations. This will be discussed in the section below.

An example of a simple configuration file class would look as follows:
\code{.cpp}
class NewConfig
	: public JSONConfigProperties<NewConfig, PropNames<"Param1", "Param2">, std::string, float>
{
	public:
		static constexpr FixedString ConfigType = "NewConfiguration";

		// Constructor. Forward json data to JSONConfigProperties.
		// Note the default values passed along. They will be used to construct the
		// parameters should no associated name be found in the conf JSON structure.
		// Should less default values than parameters be passed, the constructor will
		// throw an exception if the options cannot be found in th conf JSON structure
		NewConfig(const nlohmann::json &conf)
			: JSONConfigProperties(conf, "defaultString", 0.f)
		{}

		// Functions to easily access parameters
		constexpr const int &param1() const
		{	return this->getPropertyByName<"Param1">();	}
		constexpr int &param1()
		{	return this->getPropertyByName<"Param1">();	}

		constexpr const std::string &param2() const
		{	return this->getPropertyByName<"Param2">();	}
		constexpr std::string &param2()
		{	return this->getPropertyByName<"Param2">();	}
};
\endcode

\subsection Engine Configuration

As stated above, Engine Configuration is slightly different. For this, we use EngineConfig as a template base class:
\code{.cpp}
class NewEngineConfig
	: public EngineConfig<NewEngineConfig, PropNames<"Param1", "Param2">, std::string, float>
{
	public:
		static constexpr FixedString ConfigType = "NewEngineConfig";

		// Define the Engine type, as well as a default name
		static constexpr FixedString DefEngineType = "gazebo_grpc";
		static constexpr std::string_view DefEngineName = "gazebo_engine";

		// Constructor. Forward json data to EngineConfig.
		// Note the default values passed along. They will be used to construct the
		// parameters should no associated name be found in the conf JSON structure.
		// Should less default values than parameters be passed, the constructor will
		// throw an exception if the options cannot be found in th conf JSON structure
		NewEngineConfig(config_storage_t &data)
			: EngineConfig(data.Data, "defaultString", 0.f)
		{}

		// Functions to easily access parameters
		constexpr const int &param1() const
		{	return this->getPropertyByName<"Param1">();	}
		constexpr int &param1()
		{	return this->getPropertyByName<"Param1">();	}

		constexpr const std::string &param2() const
		{	return this->getPropertyByName<"Param2">();	}
		constexpr std::string &param2()
		{	return this->getPropertyByName<"Param2">();	}
};
\endcode

A slightly more complex configuration class is required when creating engine communication protocols, such as a JSON or Grpc communication
base. Here, the Engine Configuration must not only be modified, but developers must be given an option to append additional properties
to the configuration, based on the final engine type. The solution is to create a templated configuration:

\code{.cpp}
template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class NewEngineProtocolConfig
	: public EngineConfig<CONFIG, MultiPropNames<PropNames<"Param1", "Param2">, PROP_NAMES>, std::string, float, PROPERTIES...>
{
	public:
		static constexpr FixedString ConfigType = "NewConfiguration";

		// Constructor. Forward json data to EngineConfig.
		// Note the default values passed along. They will be used to construct the
		// parameters should no associated name be found in the conf JSON structure.
		// Should less default values than parameters be passed, the constructor will
		// throw an exception if the options cannot be found in th conf JSON structure
		template<class ...PROPERTIES_T>
		NewEngineProtocolConfig(config_storage_t &data, PROPERTIES_T &&...defaults)
			: EngineConfig(data.Data, "defaultString", 0.f, std::forward<PROPERTIES_T>(defaults)...)
		{}

		// Functions to easily access parameters
		constexpr const int &param1() const
		{	return this->getPropertyByName<"Param1">();	}
		constexpr int &param1()
		{	return this->getPropertyByName<"Param1">();	}

		constexpr const std::string &param2() const
		{	return this->getPropertyByName<"Param2">();	}
		constexpr std::string &param2()
		{	return this->getPropertyByName<"Param2">();	}
};
\endcode

 */

#endif // BASE_CONFIG_H
