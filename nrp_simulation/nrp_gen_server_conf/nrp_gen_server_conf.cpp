//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include <fstream>
#include <iostream>

#include "nrp_simulation/config/server_config.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		NRPLogger::SPDErrLogDefault(std::string(argv[0]) + ": Missing argument for server config path");
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
		NRPException::logCreate(e, std::string(argv[0]) + ": Failed to write config file");
		return -3;
	}

	return 0;
}
