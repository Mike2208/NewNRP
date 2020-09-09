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
