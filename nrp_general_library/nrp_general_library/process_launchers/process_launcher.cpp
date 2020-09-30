#include "nrp_general_library/process_launchers/process_launcher.h"


LaunchCommandInterface *ProcessLauncherInterface::launchCommand() const
{	return this->_launchCmd.get();	}

bool ProcessLauncherInterface::checkEnvVar(const std::string &envVar)
{	return (envVar.find("=", 1) != envVar.npos);	}

std::tuple<std::string, std::string> ProcessLauncherInterface::splitEnvVar(const std::string &envVar)
{
	const auto eqLocation = envVar.find("=", 1);
	if(eqLocation == envVar.npos)
		return std::tuple<std::string, std::string>();

	return std::tuple<std::string, std::string>(envVar.substr(0, eqLocation), envVar.substr(eqLocation+1, envVar.length()));
}
