#include <fstream>
#include <iostream>

#include "nrp_simulation/config/server_config.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << argv[0] << ": Missing argument for server config path" << std::endl;
		return -1;
	}

	const char *const servCfgFileName = argv[1];

	std::fstream servCfgFile(servCfgFileName, std::ios::out | std::ios::trunc);
	if(servCfgFile.fail())
	{
		std::cerr << argv[0] << ": Failed to open file server config file \"" << servCfgFileName << "\"\n";
		return -2;
	}

	try
	{
		auto servCfg = ServerConfig(nlohmann::json());
		servCfgFile << servCfg.writeConfig().dump(1, '\t') << std::endl;
	}
	catch(std::exception &e)
	{
		std::cerr << argv[0] << ": Failed to write config file \"" << e.what() << "\"\n";
		return -3;
	}

	return 0;
}
