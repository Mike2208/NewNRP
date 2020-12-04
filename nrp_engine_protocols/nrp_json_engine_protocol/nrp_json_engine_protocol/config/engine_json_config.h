#ifndef ENGINE_JSON_CONFIG_H
#define ENGINE_JSON_CONFIG_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/config/simulation_config.h"

struct EngineJSONConfigConst
{
		/*!
		 * \brief Address from which the engine server sends/receives data
		 */
		static constexpr FixedString EngineServerAddress = "ServerAddress";
		static constexpr std::string_view DefEngineServerAddress = "localhost:9002";

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

		using EJCfgNames = PropNames<EngineServerAddress, EngineRegistrationServerAddress>;
};

/*!
 *  \brief Configuration for engine communication via JSON REST server
 *  \tparam CONFIG Final Class derived from this EngineJSONConfig Class
 *  \tparam PROPERTIES Additional Engine Properties
 */
template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class EngineJSONConfig
        : public EngineConfig<CONFIG, MultiPropNames<EngineJSONConfigConst::EJCfgNames, PROP_NAMES>, PropCfg<std::string,false>, std::string, PROPERTIES...>,
          public EngineJSONConfigConst
{
		using engine_config_t = EngineConfig<CONFIG, MultiPropNames<EngineJSONConfigConst::EJCfgNames, PROP_NAMES>, PropCfg<std::string,false>, std::string, PROPERTIES...>;
	public:
		/*!
		 * \brief Constructor. Takes configuration data from the main SimulationConfig class. Will register itself with said class, so that anytime the configuration is saved, any changes made by GazeboConfig are passed along
		 * \tparam T Additional configuration
		 * \param config Engine Configuration, taken from the SimulationConfig class
		 * \param defaultServerPort Port to be used if no explicit port configuration is present in config
		 * \param properties Configuration of any additional PROPERTIES
		 */
		template<class ...T>
		EngineJSONConfig(EngineConfigConst::config_storage_t &config, T &&...properties)
		    : EngineJSONConfig<CONFIG, PROP_NAMES, PROPERTIES...>(config.Data, std::forward<T>(properties)...)
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
		EngineJSONConfig(const nlohmann::json &data, T &&...properties)
		    : engine_config_t(data,
		                      CONFIG::DefEngineServerAddress.data(), CONFIG::DefEngineRegistrationServerAddress.data(),
		                      std::forward<T>(properties)...)
		{}

		const std::string &engineServerAddress() const
		{	return this->template getPropertyByName<EngineJSONConfig::EngineServerAddress, std::string>();	}

		std::string &engineServerAddress()
		{	return this->template getPropertyByName<EngineJSONConfig::EngineServerAddress, std::string>();	}

		const std::string &engineRegistrationServerAddress() const
		{	return this->template getPropertyByName<EngineJSONConfig::EngineRegistrationServerAddress, std::string>();	}

		std::string &engineRegistrationServerAddress()
		{	return this->template getPropertyByName<EngineJSONConfig::EngineRegistrationServerAddress, std::string>();	}

		EngineConfigConst::string_vector_t allEngineProcEnvParams() const override
		{	return this->userProcEnvParams();	}

		EngineConfigConst::string_vector_t allEngineProcStartParams() const override
		{
			EngineConfigConst::string_vector_t startParams = this->userProcStartParams();

			// Add JSON registration Server address (will be used by EngineJSONServer)
			startParams.push_back(std::string("--") + EngineJSONConfig::EngineNameArg.data() + "=" + this->engineName());

			// Add JSON Server address (will be used by EngineJSONServer)
			startParams.push_back(std::string("--") + EngineJSONConfig::EngineServerAddrArg.data() + "=" + this->engineServerAddress());

			// Add JSON registration Server address (will be used by EngineJSONServer)
			startParams.push_back(std::string("--") + EngineJSONConfig::EngineRegistrationServerAddrArg.data() + "=" + this->engineRegistrationServerAddress());

			return startParams;
		}
};


#endif // ENGINE_JSON_CONFIG_H
