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

#ifndef ENGINE_LAUNCHER_MANAGER_H
#define ENGINE_LAUNCHER_MANAGER_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"

#include <set>

/*!
 * \brief Engine Launcher Manager. Used to register, and find engine launchers
 */
class EngineLauncherManager
        : public PtrTemplates<EngineLauncherManager>
{
		using launcher_set_t = std::set<EngineLauncherInterfaceSharedPtr>;

	public:
		/*!
		 * \brief Register launcher
		 * \param launcher Launcher to register
		 */
		void registerLauncher(const EngineLauncherInterfaceSharedPtr &launcher);

		/*!
		 * \brief Finds a Launcher via the given name
		 * \param name Name of Launcher
		 * \return Returns pointer to the Launcher if available, nullptr otherwise
		 */
		EngineLauncherInterfaceSharedPtr findLauncher(const EngineLauncherInterface::engine_type_t &name) const;

	private:
		/*!
		 * \brief All available launchers
		 */
		launcher_set_t _launchers;
};

using EngineLauncherManagerSharedPtr = EngineLauncherManager::shared_ptr;
using EngineLauncherManagerConstSharedPtr = EngineLauncherManager::const_shared_ptr;

#endif // ENGINE_LAUNCHER_MANAGER_H
