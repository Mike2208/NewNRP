#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H

#include "nrp_general_library/config/base_config.h"
#include "nrp_general_library/config/config_formats/json_config_properties.h"
#include "nrp_general_library/config/config_storage.h"

#include <filesystem>

class SimulationConfig;

struct SimulationConfigConst
{
	using config_storage_t = ConfigStorage<nlohmann::json>;
	using tf_configs_t = std::vector<config_storage_t>;

	/*!
	 * \brief SimulationConfig Type
	 */
	static constexpr FixedString ConfigType = "SimulationConfig";

	/*!
	 * \brief Time range (in s) for which engine completion is considered equal.
	 * Due to numerical rounding, a time range must be specified
	 * in which completion times are considered equal. Should be a low value
	 */
	static constexpr FixedString ApproximateTimeRange = "ApproximateTimeRange";
	static constexpr float DefApproximateTimeRange = 0.0001f;

	/*!
	 * \brief Experiment Timeout (in seconds). 0 means no timeout
	 */
	static constexpr FixedString SimulationTimeout = "SimulationTimeout";
	static constexpr unsigned int DefSimulationTimeout = 0;

	/*!
	 * \brief Simulator configuration. Can vary depending on the type of brain
	 */
	static constexpr FixedString EngineSimulatorsConfig = "EngineConfigs";
	static const tf_configs_t DefEngineSimulatorsConfig;

	/*!
	 * \brief Simulator configuration. Can vary depending on the type of physics engine
	 */
	static constexpr FixedString TFArrayConfig = "TransceiverFunctions";
	static const tf_configs_t DefTFArrayConfig;

	/*!
	 * \brief Name of simulation
	 */
	static constexpr FixedString SimName = "SimulationName";

	using SPropNames = PropNames<SimulationConfigConst::ApproximateTimeRange, SimulationConfigConst::SimulationTimeout,
	                             SimulationConfigConst::EngineSimulatorsConfig, SimulationConfigConst::TFArrayConfig,
	                             SimName>;

	using SProps = JSONConfigProperties<SimulationConfig, SimulationConfigConst::SPropNames,
	                                    float, unsigned int, std::vector<SimulationConfigConst::config_storage_t>,
	                                    std::vector<SimulationConfigConst::config_storage_t>, std::string >;
};

/*!
 * \brief Contains information regarding single simulation
 */
class SimulationConfig
        : public SimulationConfigConst,
          public SimulationConfigConst::SProps
{
	public:
		SimulationConfig(const nlohmann::json &configuration);

		const std::string &name() const;
		std::string &name();

		float approximateTimeRange() const;
		float &approximateTimeRange();

		unsigned int simulationTimeOut() const;
		unsigned int &simulationTimeOut();

		const tf_configs_t &engineConfigs() const;
		tf_configs_t &engineConfigs();

		const tf_configs_t &transceiverFunctionConfigs() const;
		tf_configs_t &transceiverFunctionConfigs();

	private:
};

using SimulationConfigSharedPtr = std::shared_ptr<SimulationConfig>;
using SimulationConfigConstSharedPtr = std::shared_ptr<const SimulationConfig>;

#endif // SIMULATION_CONFIG_H
