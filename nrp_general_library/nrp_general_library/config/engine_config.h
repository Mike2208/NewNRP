#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include "nrp_general_library/config/config_formats/json_config_properties.h"
#include "nrp_general_library/config/config_storage.h"
#include "nrp_general_library/config/simulation_config.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include <concepts>

/*!
 * \brief Constants used by EngineConfig
 */
struct EngineConfigConst
{
	using config_storage_t = SimulationConfigConst::config_storage_t;
	using string_vector_t = std::vector<std::string>;

	/*!
	 * \brief Engine type. Used by EngineLauncherManager to select correct engine
	 */
	static constexpr FixedString EngineType = "EngineType";
	static constexpr std::string_view DefEngineType = "invalid";

	/*!
	 * \brief Name of the engine
	 */
	static constexpr FixedString EngineName = "EngineName";
	static constexpr std::string_view DefEngineName = "engine";

	/*!
	 * \brief Engine type. Used by EngineLauncherManager to select correct engine
	 */
	static constexpr FixedString EngineLaunchCmd = "EngineLaunchCommand";
	static constexpr FixedString DefEngineLaunchCmd = "Default";

	/*!
	 * \brief Engine Timestep (in s)
	 */
	static constexpr FixedString EngineTimestep = "EngineTimestep";
	static constexpr float DefEngineTimestep = 0.01f;

	/*!
	 * \brief Engine Timeout (in seconds). How long the simulator should wait for the completion of the engine runStep. 0 or negative values are interpreted as no timeout
	 */
	static constexpr FixedString EngineCommandTimeout = "EngineCommandTimeout";
	static constexpr float DefEngineCommandTimeout = 0;

	/*!
	 * \brief Engine Process Environment Parameters
	 */
	static constexpr FixedString EngineProcEnvParams = "EngineEnvParams";
	static const string_vector_t DefEngineProcEnvParams;

	/*!
	 * \brief Engine Process Launch command
	 */
	static constexpr FixedString EngineProcCmd = "EngineProcCmd";
	static constexpr std::string_view DefEngineProcCmd = "";

	/*!
	 * \brief Engine Process Start Parameters
	 */
	static constexpr FixedString EngineProcStartParams = "EngineProcStartParams";
	static const string_vector_t DefEngineProcStartParams;

	using ECfgPropNames = PropNames<EngineType, EngineName, EngineLaunchCmd, EngineTimestep, EngineCommandTimeout, EngineProcEnvParams, EngineProcStartParams, EngineProcCmd>;

	template<class CONFIG, class PROP_NAMES, class ...PROPERTIES>
	using ECfgProps = JSONConfigProperties<CONFIG, MultiPropNames<EngineConfigConst::ECfgPropNames, PROP_NAMES>,
	                                       std::string, std::string, std::string, float, float, EngineConfigConst::string_vector_t,
	                                       EngineConfigConst::string_vector_t, std::string, PROPERTIES...>;
};

/*!
 * \brief Base class of all EngineConfigs
 */
class EngineConfigGeneral
        : public EngineConfigConst
{
	public:
		virtual ~EngineConfigGeneral() = default;

		/*!
		 * \brief Get Engine Name
		 */
		virtual const std::string &engineName() const = 0;
		std::string &engineName();

		/*!
		 * \brief Get Engine Launch command
		 */
		virtual const std::string &engineLaunchCmd() const = 0;
		std::string &engineLaunchCmd();

		/*!
		 * \brief Get Engine Timestep
		 */
		virtual const float &engineTimestep() const = 0;
		float &engineTimestep();

		/*!
		 * \brief Get maximmum time the engine runstep may take (in seconds)
		 */
		virtual const float &engineCommandTimeout() const = 0;
		float &engineCommandTimeout();

		/*!
		 * \brief Get Engine Process Environment variables defined in config
		 */
		virtual const string_vector_t &userProcEnvParams() const = 0;
		string_vector_t &userProcEnvParams();

		/*!
		 * \brief Get Engine Process Command
		 */
		virtual const std::string &engineProcCmd() const = 0;
		std::string &engineProcCmd();

		/*!
		 * \brief Get Engine Process startup parameters defined in config
		 */
		virtual const string_vector_t &userProcStartParams() const = 0;
		string_vector_t &userProcStartParams();

		/*!
		 * \brief Get all Engine Process Environment variables.
		 *        May be overriden by engines if additional environment variables are required that are not stored in engineProcEnvParams.
		 */
		virtual string_vector_t allEngineProcEnvParams() const;

		/*!
		 * \brief Get all Engine Process Startup parameters.
		 *        May be overriden by engines if additional startup parameters are required that are not stored in engineProcStartParams.
		 */
		virtual string_vector_t allEngineProcStartParams() const;
};

template<class CONFIG, PROP_NAMES_C PROP_NAMES, class ...PROPERTIES>
class EngineConfig
        : public EngineConfigConst::ECfgProps<CONFIG, PROP_NAMES, PROPERTIES...>,
          public EngineConfigGeneral
{
	public:
		using json_config_properties_t = EngineConfigConst::ECfgProps<CONFIG, PROP_NAMES, PROPERTIES...>;

		template<class ...T>
		EngineConfig(config_storage_t &data, T &&...properties)
		    : EngineConfig(data.Data, std::forward<T>(properties)...)
		{	data.Config = dynamic_cast<CONFIG*>(this);	}

		template<class ...T>
		EngineConfig(const nlohmann::json &data, T &&...properties)
		    : json_config_properties_t(data,
		                               CONFIG::DefEngineType.data(),
		                               CONFIG::DefEngineName.data(),
		                               CONFIG::DefEngineLaunchCmd.data(),
		                               CONFIG::DefEngineTimestep,
		                               CONFIG::DefEngineCommandTimeout,
		                               CONFIG::DefEngineProcEnvParams,
		                               CONFIG::DefEngineProcStartParams,
		                               CONFIG::DefEngineProcCmd.data(),
		                               std::forward<T>(properties)...)
		{}

		const std::string &engineName() const override final
		{	return this->template getPropertyByName<EngineName>();	}

		std::string &engineName()
		{	return this->EngineConfigGeneral::engineName();	}

		const std::string &engineLaunchCmd() const override final
		{	return this->template getPropertyByName<EngineLaunchCmd>();	}

		std::string &engineLaunchCmd()
		{	return this->EngineConfigGeneral::engineLaunchCmd();	}

		const float &engineTimestep() const override final
		{	return this->template getPropertyByName<EngineTimestep>();	}

		float &engineTimestep()
		{	return this->EngineConfigGeneral::engineTimestep();	}

		const float &engineCommandTimeout() const override final
		{	return this->template getPropertyByName<EngineCommandTimeout>();	}

		float &engineRunStepTimeout()
		{	return this->EngineConfigGeneral::engineRunStepTimeout();	}

		const string_vector_t &userProcEnvParams() const override final
		{	return this->template getPropertyByName<EngineProcEnvParams>();	}

		string_vector_t &userProcEnvParams()
		{	return this->EngineConfigGeneral::userProcEnvParams();	}

		const std::string &engineProcCmd() const override final
		{	return this->template getPropertyByName<EngineProcCmd>();	}

		std::string &engineProcCmd()
		{	return this->EngineConfigGeneral::engineProcCmd();	}

		const string_vector_t &userProcStartParams() const override final
		{	return this->template getPropertyByName<EngineProcStartParams>();	}

		string_vector_t &userProcStartParams()
		{	return this->EngineConfigGeneral::userProcStartParams();	}
};

template<class T>
concept ENGINE_CONFIG_C = requires (EngineConfigConst::config_storage_t &config) {
    std::derived_from<T, EngineConfigGeneral>;
    { T(config) };
};

#endif // ENGINE_CONFIG_H
