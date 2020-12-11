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

#endif // PLUGIN_MANAGER_H
