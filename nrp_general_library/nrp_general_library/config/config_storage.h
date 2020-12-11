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

#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include "nrp_general_library/config/base_config.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <nlohmann/json.hpp>

/*!
 * \brief Configuration Storage. Allows for storing a configuration without knowing the correct config class. Usefull for engines, where the config is loaded later
 * \tparam FORMAT Configuration storage format
 */
template<class FORMAT = nlohmann::json>
struct ConfigStorage
        : public BaseConfig<FORMAT>
{
	static constexpr std::string_view ConfigType = "ConfigStorage";

	/*!
	 * \brief Object that is read from the config file
	 */
	FORMAT Data;

	/*!
	 * \brief Config pointer. Will be used to update the config file
	 */
	const BaseConfig<FORMAT> *Config = nullptr;

	/*!
	 *	\brief Operator. Constructs an object from the given data
	 *	\tparam T Class to convert to FORMAT object
	 *	\param _Data Raw config data
	 */
	template<class T>
	requires std::is_convertible_v<T, FORMAT>
	ConfigStorage(const T &_Data)
	    : BaseConfig<FORMAT>(ConfigType.data()),
	      Data(_Data)
	{}

	ConfigStorage()
	    : BaseConfig<FORMAT>(ConfigType.data())
	{}

	virtual ~ConfigStorage() = default;

	/*!
	 * \brief Converts EngineConfigHolder to T object. If callback function is registered, use that to create T object, else use EngineConfigHolder::Data
	 */
	operator FORMAT() const
	{
		if(this->Config != nullptr)
			return this->Config->writeConfig();
		else
			return this->Data;
	}

	virtual FORMAT writeConfig() const override
	{
		return FORMAT(*this);
	}
};

void from_json(const nlohmann::json &j, ConfigStorage<nlohmann::json> &eng);

#endif // CONFIG_STORAGE_H
