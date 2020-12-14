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
		/*!
		 * \brief Engine Process status
		 */
		enum ENGINE_RUNNING_STATUS
		{
			UNKNOWN = -1,
			RUNNING,
			STOPPED
		};

		LaunchCommandInterface() = default;
		virtual ~LaunchCommandInterface() = default;

		/*!
		 * \brief Fork a new process for the given engine. Will read environment variables and start params from engineConfig
		 * The function should take the environment parameters and start parameters defined in engineConfig, and append any
		 * additional strings defined in additionalEnvParams and additionalStartParams before starting the Engine specified in
		 * engineConfig. If appendParentEnv is set to true, use the parent environment in the forked child. If set to false, scrub the
		 * environmet before continuing
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
		 * \brief Get the current engine process status. If status cannot be retrieved, return ENGINE_RUNNING_STATUS::UNKNOWN
		 * \return Returns status as enum ProcessLauncherInterface::ENGINE_RUNNING_STATUS
		 */
		virtual ENGINE_RUNNING_STATUS getProcessStatus()
		{	return UNKNOWN;	}

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
