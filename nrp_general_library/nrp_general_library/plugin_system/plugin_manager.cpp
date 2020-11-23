#include "nrp_general_library/plugin_system/plugin_manager.h"

#include "nrp_general_library/plugin_system/plugin.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <dlfcn.h>
#include <iostream>

using engine_launch_fcn_t = NRP_ENGINE_LAUNCH_FCN_T;

EngineLauncherInterface::unique_ptr PluginManager::loadPlugin(const std::string &pluginLibFile)
{
	dlerror();	// Clear previous error msgs

	// Try loading plugin with given paths
	void *pLibHandle = nullptr;
	for(const auto &path : this->_pluginPaths)
	{
		const std::string fileName = path.empty() ? pluginLibFile : (path/pluginLibFile).c_str();

		pLibHandle = dlopen(fileName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
		if(pLibHandle != nullptr)
			break;
	}

	// Print error if opening failed
	if(pLibHandle == nullptr)
	{
		const auto dlerr = dlerror();

		NRPLogger::SPDErrLogDefault("Unable to load plugin library \"" + pluginLibFile + "\"" + (dlerr ? std::string(": ")+dlerr : ""));

		return nullptr;
	}

	// Save stored library
	this->_loadedLibs.emplace(pluginLibFile, pLibHandle);

	// Find EngineLauncherInterface function in library
	engine_launch_fcn_t *pLaunchFcn = reinterpret_cast<engine_launch_fcn_t*>(dlsym(pLibHandle, CREATE_NRP_ENGINE_LAUNCHER_FCN_STR));
	if(pLaunchFcn == nullptr)
	{
		NRPLogger::SPDErrLogDefault("Plugin Library \"" + pluginLibFile + "\" does not contain an engine load creation function");
		NRPLogger::SPDErrLogDefault("Register a plugin using CREATE_NRP_ENGINE_LAUNCHER(engine_launcher_name)");

		return nullptr;
	}

	return EngineLauncherInterface::unique_ptr((*pLaunchFcn)());
}

PluginManager::~PluginManager()
{
	// Unload all plugins
	while(!this->_loadedLibs.empty())
	{
		auto curLibIt = --this->_loadedLibs.end();
		if(dlclose(curLibIt->second) != 0)
		{
			const auto errStr = dlerror();
			NRPLogger::SPDErrLogDefault("Couldn't unload plugin \"" + curLibIt->first + "\": " + errStr);
		}

		this->_loadedLibs.erase(curLibIt);
	}
}

void PluginManager::addPluginPath(const std::string &pluginPath)
{
	this->_pluginPaths.insert(--this->_pluginPaths.end(), pluginPath);
}
