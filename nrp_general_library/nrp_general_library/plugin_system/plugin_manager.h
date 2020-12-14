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

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"

#include <map>

/*!
 * \brief Loads libraries and extracts engine launchers
 */
class PluginManager
{
	public:
		/*!
		 * \brief Load a Plugin from a given library
		 * \param pluginLibFile Plugin library file (.so)
		 * \return Returns ptr to loaded EngineLauncher if found, nullptr otherwise
		 */
		EngineLauncherInterface::unique_ptr loadPlugin(const std::string &pluginLibFile);

		/*!
		 *	\brief Destructor. Unloads all plugin libraries
		 */
		~PluginManager();

		/*!
		 * \brief Adds search path under which to look for plugins
		 * \param pluginPath Path to plugins
		 */
		void addPluginPath(const std::string &pluginPath);


	private:
		/*!
		 * \brief Loaded Libraries
		 */
		std::map<std::string, void*> _loadedLibs;

		/*!
		 * \brief Plugin paths. The last element should always be an empty path,
		 * which signifies using the standard linux search method
		 */
		std::vector<std::filesystem::path> _pluginPaths = {std::filesystem::path()};
};

/*! \page plugin_system
The NRPSimulation executbable can load new engine types on startup. The names of additional Engine libraries can be supplied
via a "-p" parameter, followed by a comma separated list of engines to load. This gives users the ability to add their own
engines on startup and create new simulation configurations with them.

The NRPSimulation executbable loads one instance of PluginManager on startup, and fills it with a set of predefined search directories.
Should a user request an additional engine library be loaded, it will iterate over these locations until if finds a library file
matching the requested string. The user can request both a relative as well as an absolute path, both will be searched.

CMake is configured to load some libraries on startup. These are defines in the root CMakeLists.txt file, in the
NRP_SIMULATION_DEFAULT_ENGINE_LAUNCHERS variable, and will not have to be addded via the "-p" start parameter.

Creating C plugins is relatively straightforward. We have provided a macro to help developers create new engine libraries. The
macro is defined in nrp_general_library/plugin_system/plugin.h as CREATE_NRP_ENGINE_LAUNCHER(...). It takes the EngineLauncher of the
loaded Engine as parameter.
\bold Note that one library can only include one engine!

\code{.cpp}
CREATE_NRP_ENGINE_LAUNCHER(GazeboEngineJSONNRPClient::EngineLauncher<"gazebo_json">)
\endcode

A more detailed description of \ref EngineLauncher "EngineLaunchers" can be found \ref engines "here".

Once a plugin has been loaded, the NRPSimulation will add the newly loaded launcher to the running EngineLauncherManager, making it
available for Engine creation.
 */

#endif // PLUGIN_MANAGER_H
