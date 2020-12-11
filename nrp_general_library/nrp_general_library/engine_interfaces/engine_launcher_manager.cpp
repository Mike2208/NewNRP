//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_general_library/engine_interfaces/engine_launcher_manager.h"

#include <spdlog/spdlog.h>

void EngineLauncherManager::registerLauncher(const EngineLauncherInterfaceSharedPtr &launcher)
{
	const auto &sameLauncherPtr = this->findLauncher(launcher->engineType());
	if(sameLauncherPtr != nullptr)
	{
		// TODO: Handle error where two launchers have same name
		spdlog::warn("Launcher with same name \"" + sameLauncherPtr->engineType() + "\"already registered");
	}

	this->_launchers.emplace(launcher);
}

EngineLauncherInterfaceSharedPtr EngineLauncherManager::findLauncher(const EngineLauncherInterface::engine_type_t &type) const
{
	for(const auto &launcher : this->_launchers)
	{
		if(launcher->engineType() == type)
		{
			spdlog::info("Found launcher for engine " + type);
			return launcher;
		}
	}

	return EngineLauncherInterfaceSharedPtr(nullptr);
}
