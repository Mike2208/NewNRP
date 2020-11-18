#ifndef SIMULATION_LOOP_H
#define SIMULATION_LOOP_H

#include "nrp_general_library/config/simulation_config.h"
#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"
#include "nrp_general_library/transceiver_function/transceiver_function_manager.h"
#include "nrp_general_library/transceiver_function/transceiver_function_sorted_results.h"

#include "nrp_general_library/engine_interfaces/engine_interface.h"

/*!
 * \brief Manages simulation loop. Runs physics and brain interface, and synchronizes them via Transfer Functions
 */
class SimulationLoop
        : public PtrTemplates<SimulationLoop>
{
	public:
		using engine_interfaces_t = std::vector<EngineInterfaceSharedPtr>;

		SimulationLoop() = default;
		SimulationLoop(SimulationConfigSharedPtr config, engine_interfaces_t engines);

		/*!
		 * \brief Initialize engines before running loop
		 */
		void initLoop();

		/*!
		 * \brief Runs a single loop step
		 * \param timeStep How long the single components should run (in seconds)
		 */
		//void runLoopStep(float timeStep);

		/*!
		 * \brief Runs simulation for a total of runTime (in s)
		 * \param runTime Time (in s) to run simulation. At end, will run TransceiverFunctions
		 */
		void runLoop(float runTime);

		/*!
		 * \brief Get Simulation Time (in seconds)
		 * \return Returns time passed in simulation (in seconds)
		 */
		inline float getSimTime() const
		{	return this->_simTime;	}

	private:
		/*!
		 * \brief Configuration of simulation
		 */
		SimulationConfigSharedPtr _config;

		/*!
		 * \brief Engines
		 */
		engine_interfaces_t _engines;

		using engine_queue_t = std::multimap<float, EngineInterfaceSharedPtr>;

		/*!
		 * \brief Engine Queue. Contains all engines, sorted by time until completion
		 */
		engine_queue_t _engineQueue;

		/*!
		 * \brief TF Manager containing all TFs associated with this simulation
		 */
		TransceiverFunctionManager _tfManager;

		/*!
		 * \brief Simulated time (in seconds)
		 */
		float _simTime = 0;

		/*!
		 * \brief Initialize the TF Manager. Reads the TF Configurations from the Simulation Config, and registers the TFs
		 * \param simConfig Simulation Config
		 * \param engines Loaded Engines
		 * \return Returns initialized TransceiverFunctionManager
		 */
		static TransceiverFunctionManager initTFManager(const SimulationConfigSharedPtr &simConfig, const engine_interfaces_t &engines);

		/*!
		 * \brief Handle device intputs of specified interface
		 * \param interfacePtr Shared Pointer to interface
		 * \param results Results to be processes
		 * \return Returns result of device handling inputs
		 */
		EngineInterface::RESULT handleInputDevices(const EngineInterfaceSharedPtr &engine, const TransceiverFunctionSortedResults &results);

		friend class SimulationLoopTest_InitTFManager_Test;
};

using SimulationLoopSharedPtr = SimulationLoop::shared_ptr;
using SimulationLoopConstSharedPtr = SimulationLoop::const_shared_ptr;

#endif // SIMULATION_LOOP_H
