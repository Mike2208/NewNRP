#include "nrp_simulation/simulation/simulation_loop.h"

#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/config/transceiver_function_config.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <iostream>

SimulationLoop::SimulationLoop(SimulationConfigSharedPtr config, engine_interfaces_t engines)
    : _config(config),
      _engines(engines),
      _tfManager(SimulationLoop::initTFManager(config, _engines))
{
	TransceiverDeviceInterface::setTFInterpreter(&(this->_tfManager.getInterpreter()));

	for(const auto &curEnginePtr : this->_engines)
	{	this->_engineQueue.emplace(0, curEnginePtr);	}
}

void SimulationLoop::initLoop()
{
	for(const auto &engine : this->_engines)
	{
		try
		{
			engine->initialize();
		}
		catch(std::exception &e)
		{
			throw NRPException::logCreate(e, "Failed to initialize engine \"" + engine->engineName() + "\"");
		}
	}
}

void SimulationLoop::runLoop(float runLoopTime)
{
	const auto loopStopTime = this->_simTime + runLoopTime;
	if(this->_engineQueue.empty())
	{
		this->_simTime = loopStopTime;
		return;
	}

	std::vector<EngineInterfaceSharedPtr> processedEngines;
	while(this->_engineQueue.begin()->first < loopStopTime)
	{
		// Check which engines should finish
		const auto maxCompletionTime = this->_engineQueue.begin()->first + this->_config->approximateTimeRange();
		do
		{
			this->_simTime = this->_engineQueue.begin()->first;
			processedEngines.push_back(this->_engineQueue.begin()->second);

			this->_engineQueue.erase(this->_engineQueue.begin());
		}
		while(!this->_engineQueue.empty() && this->_engineQueue.begin()->first < maxCompletionTime);

		// Wait for engines to complete execution
		for(const auto &engine : processedEngines)
		{
			try
			{
				engine->waitForStepCompletion(engine->engineConfigGeneral()->engineCommandTimeout());
			}
			catch(std::exception &e)
			{
				throw NRPException::logCreate(e, "Engine \"" + engine->engineName() +"\" loop exceeded timeout of " +
				                              std::to_string(engine->engineConfigGeneral()->engineCommandTimeout()) + "s");
			}
		}

		// Retrive devices from processed engines
		const auto requestedDeviceIDs = this->_tfManager.updateRequestedDeviceIDs();
		EngineInterface::device_outputs_t outputDevices;
		try
		{
			for(auto &engine : processedEngines)
			{
				engine->requestOutputDevices(requestedDeviceIDs);
			}
		}
		catch(std::exception &)
		{
			// TODO: Handle failure on output device retrieval
			throw;
		}

		// Execute all TFs, and sort results according to engine
		TransceiverFunctionSortedResults results;
		for(const auto &engine : processedEngines)
		{
			auto curResults = this->_tfManager.executeActiveLinkedTFs(engine->engineName());
			results.addResults(curResults);
		}

		// Send engine inputs to corresponding interfaces
		for(const auto &engine : processedEngines)
		{
			try
			{
				this->handleInputDevices(engine, results);
			}
			catch(std::exception &e)
			{
				throw NRPException::logCreate(e, "Failed to send devices to engine \"" + engine->engineName() + "\"");
			}
		}

		// Restart engines loops
		for(auto &engine : processedEngines)
		{
			const auto trueRunTime = this->_simTime - engine->getEngineTime() + engine->getEngineTimestep();

			if(trueRunTime >= 0.0f)
			{
				try
				{
					engine->runLoopStep(trueRunTime);
				}
				catch(std::exception &e)
				{
					throw NRPException::logCreate(e, "Failed to start loop of engine \"" + engine->engineName() + "\"");
				}

				// Reinsert engines into queue
				this->_engineQueue.emplace(this->_simTime + engine->getEngineTimestep(), engine);
			}
			else
			{
				NRPLogger::SPDWarnLogDefault("Engine \"" + engine->engineName() + "\" is ahead of simulation time by " +
				                             std::to_string(engine->getEngineTime() - this->_simTime) + "s\n");

				// Wait for rest of simulation to catch up to engine
				this->_engineQueue.emplace(engine->getEngineTime(), engine);
			}

			engine = nullptr;
		}

		processedEngines.clear();
	}

	this->_simTime = loopStopTime;
}

TransceiverFunctionManager SimulationLoop::initTFManager(const SimulationConfigSharedPtr &simConfig, const engine_interfaces_t &engines)
{
	TransceiverFunctionManager newManager;

	{
		TransceiverFunctionInterpreter::engines_devices_t engineDevs;
		for(const auto &engine : engines)
			engineDevs.emplace(engine->engineName(), &(engine->getOutputDevices()));

		newManager.getInterpreter().setEngineDevices(std::move(engineDevs));
	}

	TransceiverDeviceInterface::setTFInterpreter(&newManager.getInterpreter());

	const auto &transceiverFunctions = simConfig->transceiverFunctionConfigs();
	for(const auto &tf : transceiverFunctions)
		newManager.loadTF(TransceiverFunctionConfigSharedPtr(new TransceiverFunctionConfig(tf)));

	return newManager;
}

void SimulationLoop::handleInputDevices(const EngineInterfaceSharedPtr &engine, const TransceiverFunctionSortedResults &results)
{
	// Find corresponding device inputs
	const auto interfaceResultIterator = results.find(engine->engineName());
	if(interfaceResultIterator != results.end())
		engine->handleInputDevices(interfaceResultIterator->second);

	// If no inputs are available, have interface handle empty device input list
	engine->handleInputDevices(typename EngineInterface::device_inputs_t());
}

/*! \page simulation_loop Simulation Loop
The SimulationLoop is the class responsible for managing the execution of a simulation.

On initialization, it creates a TransceiverFunctionManager to manage all user-generated TransceiverFunctions. Additionally, it runs the initialization routines of all engines.

During the simulation, several components are managed by the SimulationLoop:
- The timestep of each engine is checked, and execution is staggered accordingly
- Devices are sent to/received from engines via their respective server/client architecture
- Devices are stored inside a buffer of TransceiverFunctionManager. Updates are linked to engine timesteps
- Once an engine finishes execution, all TransceiverFunctions linked to said engine will be executed. Newly acquired devices are stored inside TransceiverFunctionManager's buffer
- The NRPClient can communicate with the SimulationLoop and request updates/changes to the simulation
- Once a timeout has occured or the NRPClient requests a shutdown, the SimulationLoop is stopped

On shutdown, each engine is issued a shutdown command to close gracefully.
 */
