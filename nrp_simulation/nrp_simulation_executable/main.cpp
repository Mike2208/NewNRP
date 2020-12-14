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

#include "nrp_general_library/config/cmake_constants.h"
#include "nrp_general_library/plugin_system/plugin_manager.h"
#include "nrp_general_library/process_launchers/process_launcher_manager.h"
#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_general_library/utils/restclient_setup.h"
#include "nrp_simulation/config/cmake_conf.h"
#include "nrp_simulation/simulation/simulation_manager.h"

#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <string.h>

void loadPlugins(const char *libName, PluginManager &pluginManager, const EngineLauncherManagerSharedPtr &engines)
{
	// Extract plugin file name and load it
	auto engineLauncher = pluginManager.loadPlugin(libName);
	if(engineLauncher == nullptr)
		throw NRPException::logCreate(std::string("Failed to load engine launcher from plugin \"") + libName + "\"");

	// Register launcher
	engines->registerLauncher(EngineLauncherInterfaceSharedPtr(engineLauncher.release()));
}

int main(int argc, char *argv[])
{	
	RestClientSetup::ensureInstance();

	// Parse start params
	auto optParser = SimulationParams::createStartParamParser();
	std::unique_ptr<cxxopts::ParseResult> startParamPtr;
	try
	{
		startParamPtr.reset(new cxxopts::ParseResult(optParser.parse(argc, argv)));
	}
	catch(cxxopts::OptionParseException &e)
	{
		// If options aren't well formed, output help and exit
		std::cout << e.what() << std::endl;
		std::cout << optParser.help();
		return 0;
	}

	auto &startParams = *startParamPtr;

	// If help output was requested, only print that, then exit
	if(startParams[SimulationParams::ParamHelp.data()].as<bool>())
	{
		std::cout << optParser.help();
		return 0;
	}

	// Setup Python
	PythonInterpreterState pythonInterp(argc, argv);

	// Create Process launchers
	MainProcessLauncherManager::shared_ptr processLaunchers(new MainProcessLauncherManager());

	// Create engine launchers
	PluginManager pluginManager;
	EngineLauncherManagerSharedPtr engines(new EngineLauncherManager());


	// Load engine launchers from default plugins
	{
		// Add plugin path to LD_LIBRARY_PATH
		pluginManager.addPluginPath(NRP_PLUGIN_INSTALL_DIR);

		// Iterate over default plugin libs, separated by ' '
		const auto defaultLaunchers = NRP_SIMULATION_DEFAULT_ENGINE_LAUNCHERS;
		for(const auto &libName : defaultLaunchers)
			loadPlugins(libName, pluginManager, engines);

		auto pluginsParam = startParams[SimulationParams::ParamPlugins.data()].as<SimulationParams::ParamPluginsT>();
		for(const auto &libName : pluginsParam)
			loadPlugins(libName.c_str(), pluginManager, engines);
	}

	// Load simulation
	SimulationManager manager = SimulationManager::createFromParams(startParams);

	// Check if configuration file was specified
	if(manager.simulationConfig() != nullptr)
	{
		spdlog::info("Config file specified, launching...\n");

		auto simLock = manager.acquireSimLock();
		manager.initSimulationLoop(engines, processLaunchers, simLock);

		manager.runSimulationUntilTimeout(simLock);
	}

	return 0;
}

/*! \page nrp_simulation NRPSimulation
This is the main NRP executable. It can be used to run an experiment locally. With it, a simulation is configured, a SimulationLoop is initialized, and a simulation is run. The process
is divided into the following steps:
- Parse input parameters
  - If a '-h' or '--help' is added to the simulation, only print the help text, then exit
- Initialize the python interpreter for TransceiverFunctions
- Setup the process launchers. Later on, the ServerConfig file will determine the correct launcher to use to start engine processes
- Load engines
  - Start the PluginManager.
  - It will load all engines defined as default on compilation.
  - Additionally, it will read the input parameters and determine whether the user requested additional engine libraries to be loaded
  - Store all engines in an EngineLauncherManager
- Use input parameters to generate a new instance of SimulationManager. This will also launch all engine processes defined in the SimulationConfig passed to NRPSimulation
- If a SimulationConfig file was given as an input parameter, initialize a SimulationLoop and run until timeout
- TODO: If no SimulationConfig was given, wait for communication from the NRPServer

To launch an experiment with NRPSimulation, the user must specify the simulation configuration file. The file format is specified under \ref simulation_config "SimulationConfig".
\code{.sh}
NRPSimulation -c simulation_config.json
\endcode


 */
