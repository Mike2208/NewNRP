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
		 * \brief Adds pluginPath to front of LD_LIBRARY_PATH
		 * \param pluginPath Path to plugins
		 * \throw Throws if unable to set LD_LIBRARY_PATH
		 */
		void addPluginPath(const std::string &pluginPath) const;


	private:
		/*!
		 * \brief Loaded Libraries
		 */
		std::map<std::string, void*> _loadedLibs;
};

#endif // PLUGIN_MANAGER_H
