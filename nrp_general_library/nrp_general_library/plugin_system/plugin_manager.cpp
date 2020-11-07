#include "nrp_general_library/plugin_system/plugin_manager.h"

#include "nrp_general_library/plugin_system/plugin.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <dlfcn.h>
#include <iostream>

using engine_launch_fcn_t = NRP_ENGINE_LAUNCH_FCN_T;

EngineLauncherInterface::unique_ptr PluginManager::loadPlugin(const std::string &pluginLibFile)
{
	dlerror();	// Clear previous error msgs
	void *pLibHandle = dlopen(pluginLibFile.data(), RTLD_LAZY | RTLD_GLOBAL);
	if(pLibHandle == nullptr)
	{
		const auto dlerr = dlerror();

		std::cerr << "Unable to load plugin library \"" + pluginLibFile + "\"";
		if(dlerr != nullptr)
			std::cerr << ": " << dlerr;

		std::cerr << std::endl;
		return nullptr;
	}

	this->_loadedLibs.emplace(pluginLibFile, pLibHandle);

	engine_launch_fcn_t *pLaunchFcn = reinterpret_cast<engine_launch_fcn_t*>(dlsym(pLibHandle, CREATE_NRP_ENGINE_LAUNCHER_FCN_STR));
	if(pLaunchFcn == nullptr)
	{
		std::cerr << "Plugin Library \"" + pluginLibFile + "\" does not contain an engine load creation function" << std::endl;
		std::cerr << "Register a plugin using CREATE_NRP_ENGINE_LAUNCHER(engine_launcher_name)" << std::endl;

		return nullptr;
	}

	return EngineLauncherInterface::unique_ptr((*pLaunchFcn)());
}

PluginManager::~PluginManager()
{
	while(!this->_loadedLibs.empty())
	{
		auto curLibIt = --this->_loadedLibs.end();
		if(dlclose(curLibIt->second) != 0)
		{
			const auto errStr = dlerror();
			std::cerr << "Couldn't unload plugin \"" + curLibIt->first + "\": " << errStr << std::endl;
		}

		this->_loadedLibs.erase(curLibIt);
	}
}

void PluginManager::addPluginPath(const std::string &pluginPath) const
{
	// Add plugin directory to search path
	const char *const pEnvLibPath = getenv("LD_LIBRARY_PATH");
	const std::string newLibPath = pEnvLibPath == nullptr ? pluginPath
	                                                      : std::string(pEnvLibPath) + ":" + pluginPath;

	if(setenv("LD_LIBRARY_PATH", newLibPath.data(), true) != 0)
	{
		throw NRPException::logCreate(std::string("Failed to add plugin path to LD_LIBRARY_PATH: ") + strerror(errno));
	}
}
