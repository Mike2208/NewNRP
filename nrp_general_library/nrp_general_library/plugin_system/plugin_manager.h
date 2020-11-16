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
