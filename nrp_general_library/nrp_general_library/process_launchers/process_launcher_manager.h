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

#ifndef PROCESS_LAUNCHER_MANAGER_H
#define PROCESS_LAUNCHER_MANAGER_H

#include "nrp_general_library/process_launchers/process_launcher.h"
#include "nrp_general_library/process_launchers/process_launcher_basic.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <iostream>
#include <map>

/*!
 * \brief Class to manage process managers
 */
template<class ...PROCESS_LAUNCHERS>
class ProcessLauncherManager
        : public PtrTemplates<ProcessLauncherManager<PROCESS_LAUNCHERS...> >
{
	public:
		/*!
		 * \brief Constructor. Registers all Process Launchers for further use
		 */
		ProcessLauncherManager()
		{	this->registerProcessLauncher<PROCESS_LAUNCHERS...>();	}
		~ProcessLauncherManager() = default;

		// Delete copy and move constructors
		ProcessLauncherManager(const ProcessLauncherManager&) = delete;
		ProcessLauncherManager(ProcessLauncherManager&&) = delete;

		ProcessLauncherManager &operator=(ProcessLauncherManager&&) = delete;
		ProcessLauncherManager &operator=(const ProcessLauncherManager&) = delete;

		/*!
		 * \brief Create a new process launcher
		 * \param launcherType Name of launcher
		 * \return Returns ptr to launcher
		 */
		ProcessLauncherInterface::unique_ptr createProcessLauncher(const std::string &launcherType) const
		{
			//ProcessLauncherManager *const manager = ProcessLauncherManager::getInstance();

			const auto launcherIterator = this->_processLaunchers.find(launcherType);
			if(launcherIterator == this->_processLaunchers.end())
				throw NRPException::logCreate("Could not find process launcher of type \"" + launcherType + "\"");

			return launcherIterator->second->createLauncher();
		}

		void registerProcessLauncher(ProcessLauncherInterface::unique_ptr &&launcher)
		{	this->_processLaunchers.emplace(launcher->launcherName(), std::move(launcher));	}

	private:

		/*!
		 * \brief All Process Launchers
		 */
		std::map<std::string, ProcessLauncherInterface::unique_ptr> _processLaunchers;

		/*!
		 *	\brief Register process launchers specified in the template on startup
		 *	\tparam PROCESS_LAUNCHER Launcher class to register
		 *	\tparam REST Remaining launchers to register
		 */
		template<PROCESS_LAUNCHER_C PROCESS_LAUNCHER, class ...REST>
		void registerProcessLauncher()
		{
			ProcessLauncherInterface::unique_ptr launcher(new PROCESS_LAUNCHER());
			this->registerProcessLauncher(std::move(launcher));

			if constexpr (sizeof... (REST) > 0)
			{	this->registerProcessLauncher<REST...>();	}
		}
};

/*!
 * \brief Type to manage all available process launchers
 */
using MainProcessLauncherManager = ProcessLauncherManager<ProcessLauncherBasic>;

using MainProcessLauncherManagerSharedPtr = MainProcessLauncherManager::shared_ptr;
using MainProcessLauncherManagerConstSharedPtr = MainProcessLauncherManager::const_shared_ptr;

/*! \page process_launcher "Process Launcher"

\section Process Launchers
The ProcessLauncher is in control of spawning Engines in individual computational environments. Depending on the desired behavior, engines can be
created in separate processes, containers, or machines. The desired behavior can be set in the ServerConfig file, located in /etc/nrp/nrp.conf.

Default settings specify forking the NRPSimulation and launching the engine in the newly spawned child process. This setup suffices for local
simulation execution, however a more complex launching process may be desired for online simulation.

Developers can create new process launchers by creating a new class derived from the ProcessLauncher template. This new class must define the
different launch commands available to this type of machine, as well as the name of the configuration. Also, the new class must be made visible
to the NRPSimulation executable. It must be added to the MainProcessLauncherManager definition in process_launcher_manager.h.

Once this is done, the user can set the NRPSimulation to utilize the new behavior by changing the ProcessLauncher parameter in the ServerConfig
file mentioned above.

An example of a new process launcher is shown here. Note the `LAUNCH_CMD#`, which will be discussed later.
\code{.cpp}
// Create a new ProcessLauncher. This launcher can be selected by changing the ProcessLauncher parameter in the ServerConfig file to "NewBehavior",
// as defined below
class NewProcessLauncher
	: public ProcessLauncher<NewProcessLauncher, "NewBehavior", LAUNCH_CMD1, LAUNCH_CMD2, ...>
{}
\endcode

Add it to the MainProcessLauncherManager along with the already existing basic ProcessLauncher:
\code{.cpp}
...
// Include header with defined class
#include "new_process_launcher.h"
...
using MainProcessLauncherManager = ProcessLauncherManager<ProcessLauncherBasic, NewProcessLauncher>;
\endcode

\bold All ProcessLaunchers must contain at least one Launch Command associated with a default Launch behavior. It must be labeled "Default"

\subsection Launch Commands

The above example mentions a series of `LAUNCH_CMD#`s. In these classes, the actual launching process is declared. Thus, a single ProcessLauncher
behavior can actually define multiple types of Engine launch commands. All ProcessLaunchers must define at least one LaunchCommand, the default.
Should an Engine require non-default startup methods, these can be added as `LAUNCH_CMD`s to the relevant ProcessLaunchers.

To define LaunchCommands, developers can create a new class, derived from LaunchCommand. They must declare the virtual function described below.
For an additional example, look at BasicFork, the default launch command used by ProcessLauncherBasic.

\code{.cpp}
// Define a new command. Engines can use it by setting the EngineLaunchCommand in their EngineConfig to "NewCommand"
class NewLaunchCommand
		: public LaunchCommand<"NewCommand">
{
	public:
		// Launch a new engine. The function should take the environment parameters and start parameters defined in engineConfig,
		// and append any additional strings defined in additionalEnvParams and additionalStartParams before starting the
		// Engine specified in engineConfig. If appendParentEnv is set to true, use the parent environment in the forked child.
		// If set to false, scrub it before continuing
		pid_t launchEngineProcess(const EngineConfigGeneral &engineConfig, const EngineConfigConst::string_vector_t &additionalEnvParams,
								  const EngineConfigConst::string_vector_t &additionalStartParams, bool appendParentEnv) override;

		// Stop the engine. Note that this command should be run after the Engine's shutdown routinges have already been called.
		// It should try to gracefully quit the engine process. Should the Engine process not have shut down after killWait seconds,
		// forcefully shut down the process, e.g. by sending a SIGKILL command
		pid_t stopEngineProcess(unsigned int killWait) override;
};
\endcode

 */


#endif // PROCESS_LAUNCHER_MANAGER_H
