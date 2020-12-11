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

#ifndef ENGINE_GRPC_CONFIG_H
#define ENGINE_GRPC_CONFIG_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/config/simulation_config.h"

struct EngineGRPCConfigConst
{
		/*!
		 * \brief Address from which the engine server sends/receives data
		 */
		static constexpr FixedString EngineServerAddress = "ServerAddress";
		static constexpr std::string_view DefEngineServerAddress = "localhost:9004";

		/*!
		 * \brief Address to which servers should register to
		 */
		static constexpr FixedString EngineRegistrationServerAddress = "RegistrationServerAddress";
		static constexpr std::string_view DefEngineRegistrationServerAddress = "localhost:9001";

		/*!
		 * \brief Maximum amount of tries for EngineJSONserver to bind to different ports
		 */
		static constexpr short MaxAddrBindTries = 1024;

		/*!
		 * \brief Parameter name that is used to pass along the server address
		 */
		static constexpr std::string_view EngineServerAddrArg = "serverurl";

		/*!
		 * \brief Parameter name that is used to pass along the server address
		 */
		static constexpr std::string_view EngineRegistrationServerAddrArg = "regservurl";

		/*!
		 * \brief Parameter name that is used to pass along the engine name
		 */
		static constexpr std::string_view EngineNameArg = "engine";

		/*!
		 * \brief REST Server Route from which to get device information
		 */
		static constexpr std::string_view EngineServerGetDevicesRoute = "/get_device_information";

		/*!
		 * \brief REST Server Route to which to send device changes
		 */
		static constexpr std::string_view EngineServerSetDevicesRoute = "/set_device";

		/*!
		 * \brief REST Server Route to execute a single loop
		 */
		static constexpr std::string_view EngineServerRunLoopStepRoute = "/run_loop";

		/*!
		 * \brief REST Server Route for engine initialization
		 */
		static constexpr std::string_view EngineServerInitializeRoute = "/initialize";

		/*!
		 * \brief REST Server Route for engine shutdown
		 */
		static constexpr std::string_view EngineServerShutdownRoute = "/shutdown";

		/*!
		 * \brief JSON name under which the runLoopStep timeStep is saved
		 */
		static constexpr std::string_view EngineTimeStepName = "time_step";

		/*!
		 * \brief JSON name under which the runLoopStep engine time is sent
		 */
		static constexpr std::string_view EngineTimeName = "time";

		/*!
		 * \brief Content Type passed between server and client
		 */
		static constexpr std::string_view EngineServerContentType = "application/json";

		using EGCfgNames = PropNames<EngineServerAddress, EngineRegistrationServerAddress>;
};

/*!
 *  \brief Configuration for engine communication via JSON REST server
 *  \tparam CONFIG Final Class derived from this EngineGRPCConfig Class
 *  \tparam PROPERTIES Additional Engine Properties
 */
template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class EngineGRPCConfig
        : public EngineConfig<CONFIG, MultiPropNames<EngineGRPCConfigConst::EGCfgNames, PROP_NAMES>, PropCfg<std::string,false>, std::string, PROPERTIES...>,
          public EngineGRPCConfigConst
{
		using engine_config_t = EngineConfig<CONFIG, MultiPropNames<EngineGRPCConfigConst::EGCfgNames, PROP_NAMES>, PropCfg<std::string,false>, std::string, PROPERTIES...>;
	public:
		/*!
		 * \brief Constructor. Takes configuration data from the main SimulationConfig class. Will register itself with said class, so that anytime the configuration is saved, any changes made by GazeboConfig are passed along
		 * \tparam T Additional configuration
		 * \param config Engine Configuration, taken from the SimulationConfig class
		 * \param defaultServerPort Port to be used if no explicit port configuration is present in config
		 * \param properties Configuration of any additional PROPERTIES
		 */
		template<class ...T>
		EngineGRPCConfig(EngineConfigConst::config_storage_t &config, T &&...properties)
		    : EngineGRPCConfig<CONFIG, PROP_NAMES, PROPERTIES...>(config.Data, std::forward<T>(properties)...)
		{
			// Add callback function to Simulation config so that any changes made by GazeboConfig will be properly stored
			config.Config = this;
		}

		/*!
		 * \brief Constructor. Takes configuration data from the main SimulationConfig class. Will register itself with said class, so that anytime the configuration is saved, any changes made by GazeboConfig are passed along
		 * \tparam T Additional configuration
		 * \param data Configuration data
		 * \param defaultServerPort Port to be used if no explicit port configuration is present in config
		 * \param properties Configuration of any additional PROPERTIES
		 */
		template<class ...T>
		EngineGRPCConfig(const nlohmann::json &data, T &&...properties)
		    : engine_config_t(data,
		                      CONFIG::DefEngineServerAddress.data(), CONFIG::DefEngineRegistrationServerAddress.data(),
		                      std::forward<T>(properties)...)
		{}

		const std::string &engineServerAddress() const
		{	return this->template getPropertyByName<EngineGRPCConfig::EngineServerAddress, std::string>();	}

		std::string &engineServerAddress()
		{	return this->template getPropertyByName<EngineGRPCConfig::EngineServerAddress, std::string>();	}

		const std::string &engineRegistrationServerAddress() const
		{	return this->template getPropertyByName<EngineGRPCConfig::EngineRegistrationServerAddress, std::string>();	}

		std::string &engineRegistrationServerAddress()
		{	return this->template getPropertyByName<EngineGRPCConfig::EngineRegistrationServerAddress, std::string>();	}

		EngineConfigConst::string_vector_t allEngineProcEnvParams() const override
		{	return this->userProcEnvParams();	}

		EngineConfigConst::string_vector_t allEngineProcStartParams() const override
		{
			EngineConfigConst::string_vector_t startParams = this->userProcStartParams();

			// Add JSON registration Server address (will be used by EngineJSONServer)
			startParams.push_back(std::string("--") + EngineGRPCConfig::EngineNameArg.data() + "=" + this->engineName());

			// Add JSON Server address (will be used by EngineJSONServer)
			startParams.push_back(std::string("--") + EngineGRPCConfig::EngineServerAddrArg.data() + "=" + this->engineServerAddress());

			// Add JSON registration Server address (will be used by EngineJSONServer)
			startParams.push_back(std::string("--") + EngineGRPCConfig::EngineRegistrationServerAddrArg.data() + "=" + this->engineRegistrationServerAddress());

			return startParams;
		}
};


#endif // ENGINE_GRPC_CONFIG_H
