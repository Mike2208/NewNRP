#include "nrp_simulation/simulation/simulation_loop.h"

#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/config/transceiver_function_config.h"

#include <iostream>

SimulationLoop::SimulationLoop(SimulationConfigSharedPtr config, engine_interfaces_t engines)
    : _config(config),
      _engines(engines),
      _tfManager(SimulationLoop::initTFManager(config))
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
			if(engine->initialize() != EngineInterface::SUCCESS)
				throw std::runtime_error("Failed to initialize engine");
		}
		catch(std::exception &e)
		{
			const auto errMsg = "Failed to initialize engine \"" + engine->engineName() + "\": " + e.what();

			std::cerr << errMsg << std::endl;
			throw std::runtime_error(errMsg);
		}
	}
}

void SimulationLoop::runLoop(float runTime)
{
	const auto loopStopTime = this->_simTime + runTime;
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
			if(engine->waitForStepCompletion(engine->engineConfigGeneral()->engineRunStepTimeout()) != EngineInterface::SUCCESS)
			{
				// TODO: Handle loop completion failure
			}
		}

		// Retrive devices from processed engines
		const auto requestedDeviceIDs = this->_tfManager.updateRequestedDeviceIDs();
		EngineInterface::device_outputs_t outputDevices;
		try
		{
			for(auto &engine : processedEngines)
			{
				this->_tfManager.setEngineOutputDeviceData(engine->engineName(), engine->getOutputDevices(requestedDeviceIDs));
			}
		}
		catch(const std::exception &)
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
			if(this->handleInputDevices(engine, results) != EngineInterface::SUCCESS)
			{
				// TODO: Handle physics and/or brain TF error
			}
		}

		// Restart engines loops
		for(auto &engine : processedEngines)
		{
			const auto runTime = this->_simTime - engine->getEngineTime() + engine->getEngineTimestep();

			if(runTime >= 0.0f)
			{
				if(engine->runLoopStep(runTime) != EngineInterface::SUCCESS)
				{
					// TODO: Handle loop start failure
				}
			}
			else
			{
				std::cout << "Warning: Engine \"" + engine->engineName() + "\" is ahead of simulation time by "
				             + std::to_string(engine->getEngineTime() - this->_simTime) + "s\n";
			}

			// Reinsert engines into queue
			this->_engineQueue.emplace(this->_simTime + runTime, engine);

			engine = nullptr;
		}

		processedEngines.clear();
	}

	this->_simTime = loopStopTime;
}

TransceiverFunctionManager SimulationLoop::initTFManager(const SimulationConfigSharedPtr &simConfig)
{
	TransceiverFunctionManager newManager;

	TransceiverDeviceInterface::setTFInterpreter(&newManager.getInterpreter());

	const auto &transceiverFunctions = simConfig->transceiverFunctionConfigs();
	for(const auto &tf : transceiverFunctions)
		newManager.loadTF(TransceiverFunctionConfigSharedPtr(new TransceiverFunctionConfig(tf)));

	return newManager;
}

EngineInterface::RESULT SimulationLoop::handleInputDevices(const EngineInterfaceSharedPtr &engine, const TransceiverFunctionSortedResults &results)
{
	// Find corresponding device inputs
	const auto interfaceResultIterator = results.find(engine->engineName());
	if(interfaceResultIterator != results.end())
		return engine->handleInputDevices(interfaceResultIterator->second);

	// If no inputs are available, have interface handle empty device input list
	return engine->handleInputDevices(typename EngineInterface::device_inputs_t());
}

/*! \page simulation_loop Simulation Loop
The SimulationLoop is the class responsible for managing the execution of a simulation.

On initialization, it creates a TransceiverFunctionManager to manage all user-generated TransceiverFunctions. Additionally, it runs the initialization routines of all engines

During the simulation, several components are managed by the SimulationLoop:
- The timestep of each engine is checked, and execution is staggered accordingly
- Devices are sent to/received from engines via their respective server/client architecture
- Devices are stored inside a buffer of TransceiverFunctionManager. Updates are linked to engine timesteps
- Once an engine finishes execution, all TransceiverFunctions linked to said engine will be executed. Newly acquired devices are stored inside TransceiverFunctionManager's buffer
- The NRPClient can communicate with the SimulationLoop and request updates/changes to the simulation
- Once a timeout has occured or the NRPClient requests a shutdown, the SimulationLoop is stopped

On shutdown, each engine is issued a shutdown command to close gracefully
 */
