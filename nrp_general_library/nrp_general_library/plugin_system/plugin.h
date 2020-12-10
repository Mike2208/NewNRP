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
