#include "nrp_general_library/config_headers/nrp_cmake_constants.h"
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
This is the main execution function. Here a simulation is configured, and a SimulationLoop is initialized. The process is divided into the following steps:
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
 */
