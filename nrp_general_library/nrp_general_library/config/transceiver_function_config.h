#ifndef TRANSCEIVER_FUNCTION_CONFIG_H
#define TRANSCEIVER_FUNCTION_CONFIG_H

#include "nrp_general_library/config/base_config.h"
#include "nrp_general_library/config/config_storage.h"
#include "nrp_general_library/config/config_formats/json_config_properties.h"
#include "nrp_general_library/utils/ptr_templates.h"
#include "nrp_general_library/config/simulation_config.h"

struct TransceiverFunctionConfigConst
{
	/*!
	 * \brief Name of TF
	 */
	static constexpr FixedString Name = "Name";
	static constexpr std::string_view DefName = "TF";

	/*!
	 *	\brief Name of file containing transceiver function python script
	 */
	static constexpr FixedString File = "FileName";
	static constexpr std::string_view DefFile = "";

	/*!
	 * \brief Is this TF active? Only active TFs will be executed
	 */
	static constexpr FixedString IsActive = "IsActive";
	static constexpr bool DefIsActive = true;

	/*!
	 * \brief TransceiverFunctionConfig Type
	 */
	static constexpr FixedString ConfigType = "TransceiverFunctionConfig";
};

/*!
 * \brief Contains transfer function configuration
 */
class TransceiverFunctionConfig
        : public JSONConfigProperties<TransceiverFunctionConfig,
                                      PropNames<TransceiverFunctionConfigConst::Name, TransceiverFunctionConfigConst::File,
                                                TransceiverFunctionConfigConst::IsActive>,
                                      std::string, std::string, bool>,
          public TransceiverFunctionConfigConst,
          public PtrTemplates<TransceiverFunctionConfig>
{
	public:
		TransceiverFunctionConfig();
		TransceiverFunctionConfig(SimulationConfig::config_storage_t &config);
		TransceiverFunctionConfig(const nlohmann::json &config);

		virtual ~TransceiverFunctionConfig() override = default;

		const std::string &fileName() const;
		void setFileName(const std::string &fileName);

		const std::string &name() const;
		void setName(const std::string &name);

		bool isActive() const;
		void setIsActive(bool active);
};

using TransceiverFunctionConfigSharedPtr		= TransceiverFunctionConfig::shared_ptr;
using TransceiverFunctionConfigConstSharedPtr	= TransceiverFunctionConfig::const_shared_ptr;

#endif // TRANSCEIVER_FUNCTION_CONFIG_H
