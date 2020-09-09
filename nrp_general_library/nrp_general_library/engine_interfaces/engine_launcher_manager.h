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
