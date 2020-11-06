#ifndef LAUNCH_COMMAND_H
#define LAUNCH_COMMAND_H

#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/utils/fixed_string.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include <concepts>
#include <string>
#include <vector>

class LaunchCommandInterface
        : public PtrTemplates<LaunchCommandInterface>
{
	public:
		LaunchCommandInterface() = default;
		virtual ~LaunchCommandInterface() = default;

		/*!
		 * \brief Fork a new process for the given engine. Will read environment variables and start params from engineConfig
		 * \param engineConfig Engine Configuration. Env variables and start params take precedence over additionalEnvParams and additionalStartParams
		 * \param additionalEnvParams Additional Environment Variables for child process. Will take precedence over default env params if appendParentEnv is true
		 * \param additionalStartParams Additional Start parameters
		 * \param appendParentEnv Should parent env variables be appended to child process
		 * \return Returns PID of child process on success
		 */
		virtual pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
		                                  const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv = true) = 0;


		/*!
		 * \brief Stop a running engine process
		 * \param killWait Time (in seconds) to wait for process to quit by itself before force killing it. 0 means it will wait indefinetly
		 * \return Returns 0 on sucess, negative value on error
		 */
		virtual pid_t stopEngineProcess(unsigned int killWait) = 0;

		/*!
		 * \brief Get launch command type
		 */
		virtual std::string_view launchType() const = 0;
};

/*!
 * \brief Class for launch commands. Must be specialized further
 * \tparam LAUNCH_COMMAND Name of launch command
 */
template<FixedString LAUNCH_COMMAND>
class LaunchCommand
        : public LaunchCommandInterface
{
	public:
		static constexpr auto LaunchType = LAUNCH_COMMAND;

		std::string_view launchType() const override final
		{	return LaunchType;	}
};

template<class T>
concept LAUNCH_COMMAND_C = requires() {
    std::derived_from<T, LaunchCommandInterface>;
    {	T::LaunchType	};
};

#endif // LAUNCH_COMMAND_H
