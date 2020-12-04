#ifndef PROCESS_LAUNCHER_BASIC_H
#define PROCESS_LAUNCHER_BASIC_H

#include "nrp_general_library/config/cmake_constants.h"
#include "nrp_general_library/process_launchers/process_launcher.h"
#include "nrp_general_library/process_launchers/launch_commands/basic_fork.h"

/*!
 * \brief Basic Process Launcher, for simple process management
 */
class ProcessLauncherBasic
        : public ProcessLauncher<ProcessLauncherBasic, "Basic", BasicFork>
{
	public:	~ProcessLauncherBasic() override = default;
};

#endif // PROCESS_LAUNCHER_BASIC_H
