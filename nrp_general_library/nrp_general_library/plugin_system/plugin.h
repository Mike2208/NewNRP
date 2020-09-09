#ifndef PLUGIN_H
#define PLUGIN_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"

#define CREATE_NRP_ENGINE_LAUNCHER_FCN CreateNRPEngineLauncher
#define CREATE_NRP_ENGINE_LAUNCHER_FCN_STR "CreateNRPEngineLauncher"

// Add a macro to block creation of Engine Launcher function. Useful for testing
#ifndef DEBUG_NO_CREATE_ENGINE_LAUNCHER_FCN
/*!
 *	\brief Create a new engine launcher. Will be used to load a launcher out of a dynamically loaded library
 */
#define CREATE_NRP_ENGINE_LAUNCHER(engine_launcher_name)                    \
	extern "C" EngineLauncherInterface *CreateNRPEngineLauncher ();  \
	EngineLauncherInterface *CreateNRPEngineLauncher ()            { \
	    return new engine_launcher_name();                                  \
	}
#else
#define CREATE_NRP_ENGINE_LAUNCHER(engine_launcher_name)
#endif


#define NRP_ENGINE_LAUNCH_FCN_T EngineLauncherInterface*();

#endif // PLUGIN_H
