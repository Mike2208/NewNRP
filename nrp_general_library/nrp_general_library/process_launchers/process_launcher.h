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

#ifndef PROCESS_LAUNCHER_H
#define PROCESS_LAUNCHER_H

#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/process_launchers/launch_commands/launch_command.h"
#include "nrp_general_library/utils/fixed_string.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include <concepts>

/*!
 * \brief Functions for all process launchers
 */
class ProcessLauncherInterface
        : public PtrTemplates<ProcessLauncherInterface>
{
	public:
		// Process status options
		using ENGINE_RUNNING_STATUS = LaunchCommandInterface::ENGINE_RUNNING_STATUS;
		static constexpr auto UNKNOWN = LaunchCommandInterface::ENGINE_RUNNING_STATUS::UNKNOWN;
		static constexpr auto RUNNING = LaunchCommandInterface::ENGINE_RUNNING_STATUS::RUNNING;
		static constexpr auto STOPPED = LaunchCommandInterface::ENGINE_RUNNING_STATUS::STOPPED;

		virtual ~ProcessLauncherInterface() = default;

		/*!
		 * \brief Get name of launcher
		 */
		virtual std::string launcherName() const = 0;

		/*!
		 * \brief Create a new proces launcher
		 */
		virtual ProcessLauncherInterface::unique_ptr createLauncher() = 0;

		/*!
		 * \brief Fork a new process for the given engine. Will read environment variables and start params from engineConfig
		 * \param engineConfig Engine Configuration. Env variables and start params take precedence over additionalEnvParams and additionalStartParams
		 * \param additionalEnvParams Additional Environment Variables for child process. Will take precedence over default env params if appendParentEnv is true
		 * \param additionalStartParams Additional Start parameters
		 * \param appendParentEnv Should parent env variables be appended to child process
		 * \return Returns Process ID of child process on success
		 */
		virtual pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
		                                  const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv = true) = 0;
		/*!
		 * \brief Stop a running engine process
		 * \param killWait Time (in seconds) to wait for process to quit by itself before force killing it. 0 means it will wait indefinetly
		 * \return Returns child PID on sucess, negative value on error
		 */
		virtual pid_t stopEngineProcess(unsigned int killWait) = 0;

		/*!
		 * \brief Get the current engine process status. If status cannot be retrieved, return ENGINE_RUNNING_STATUS::UNKNOWN
		 * \return Returns status as enum ProcessLauncherInterface::ENGINE_RUNNING_STATUS
		 */
		virtual ENGINE_RUNNING_STATUS getProcessStatus()
		{	return this->_launchCmd ? this->_launchCmd->getProcessStatus() : ENGINE_RUNNING_STATUS::UNKNOWN;	}

		/*!
		 * \brief Get Launch Command. If launchEngineProcess has not yet been called, return nullptr
		 */
		LaunchCommandInterface *launchCommand() const;

	protected:
		/*!
		 * \brief Checks given Environment variable for correctness (Should contain an '=' character)
		 * \param envVar Variable to check
		 * \return Returns true if valid, false otherwise
		 */
		static bool checkEnvVar(const std::string &envVar);

		/*!
		 * \brief Split Environment variable string into variable name and value
		 * \param envVar String to split. Should have the form <VAR_NAME>=<VAR_VALUE>
		 * \return Returns tuple. First value is VAR_NAME, second one is VAR_VALUE. If envVar has an invalid form, returns an empty string for both values
		 */
		static std::tuple<std::string,std::string> splitEnvVar(const std::string &envVar);

		/*!
		 * \brief Launch Command
		 */
		LaunchCommandInterface::unique_ptr _launchCmd = nullptr;
};

template<class T>
concept PROCESS_LAUNCHER_C = requires {
    std::derived_from<T, ProcessLauncherInterface>;
    { T() };
};

/*!
 *	\brief Base class for all process launchers
 *	\tparam PROCESS_LAUNCHER Final class derived from ProcessLauncher
 *	\tparam LAUNCHER_TYPE Launcher Type as string
 */
template<class PROCESS_LAUNCHER, FixedString LAUNCHER_TYPE, LAUNCH_COMMAND_C ...LAUNCHER_COMMANDS>
class ProcessLauncher
        : public ProcessLauncherInterface
{
	public:
		static constexpr auto LauncherType = LAUNCHER_TYPE;

		~ProcessLauncher() override = default;

		ProcessLauncherInterface::unique_ptr createLauncher() override
		{	return ProcessLauncherInterface::unique_ptr(new PROCESS_LAUNCHER());	}

		std::string launcherName() const override final
		{	return std::string(LauncherType);	}

		pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
		                                  const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv = true) override final
		{
			if constexpr (sizeof...(LAUNCHER_COMMANDS) == 0)
			{	throw noLauncherFound(engineConfig.engineLaunchCmd());	}

			this->_launchCmd = ProcessLauncher::findLauncher<LAUNCHER_COMMANDS...>(engineConfig.engineLaunchCmd());
			return this->_launchCmd->launchEngineProcess(engineConfig, additionalEnvParams, additionalStartParams,appendParentEnv);
		}

		pid_t stopEngineProcess(unsigned int killWait) override final
		{	return this->_launchCmd->stopEngineProcess(killWait);	}

	private:
		template<class LAUNCH_CMD, class ...REST>
		inline LaunchCommandInterface::unique_ptr findLauncher(const std::string &launchCmd)
		{
			if(std::string(LAUNCH_CMD::LaunchType) == launchCmd)
				return LaunchCommandInterface::unique_ptr(new LAUNCH_CMD());
			else
			{
				if constexpr (sizeof...(REST) > 0)
				{	return findLauncher<REST...>(launchCmd);	}
				else
				{	throw noLauncherFound(launchCmd);	}
			}
		}

		static inline std::runtime_error noLauncherFound(const std::string &launchCmd)
		{	return std::runtime_error("Unable to find launcher with name \"" + launchCmd + "\"");	}
};

#endif // PROCESS_LAUNCHER_H
