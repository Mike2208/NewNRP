#ifndef SIMULATION_SERVER_H
#define SIMULATION_SERVER_H

#include "nrp_simulation/config/server_config.h"
#include "nrp_simulation/server/simulation_status.h"
#include "nrp_simulation/simulation/simulation_manager.h"
#include "nrp_simulation/utils/pipe_packet_communication.h"

#include <thread>
#include <unistd.h>

/*!
 * \brief SimulationServer constants
 */
struct SimulationServerConst
{
	// Commands
	/*!
	 * \brief PComm Command. Retrieves SimulationStatus struct as JSON blob
	 */
	static constexpr std::string_view GetSimStatusCommand    = "get_sim_status";

	/*!
	 * \brief PComm Command. Retrieves boolean value indicating whether simulation is running (1) or paused (0)
	 */
	static constexpr std::string_view GetSimRunningCommand   = "get_sim_running";

	/*!
	 * \brief PComm Command. Sets simulation running status
	 * Incoming:
	 * - struct SimulationRunningData
	 */
	static constexpr std::string_view PostSimRunningCommand  = "post_sim_running";

	/*!
	 * \brief PComm Command. Get data from engine.
	 * Setup:
	 * - int32_t Engine Name Length
	 * - Engine Name String. Length is same as above value
	 * - Rest of packet data will be passed to engine
	 * Outgoing:
	 * - Engine data
	 */
	static constexpr std::string_view GetEngineDataCommand = "get_engine_data";

	/*!
	 * \brief PComm Command. Set engine data.
	 * Incoming:
	 * - int32_t Engine Name Length
	 * - Engine Name String. Length is same as above value
	 * - Rest of packet data will be passed to engine
	 * Outgoing:
	 * - Engine data
	 */
	static constexpr std::string_view SetEngineDataCommand = "set_engine_data";

	/*!
	 * \brief PComm Command. Shuts engine down and kills process
	 */
	static constexpr std::string_view ShutdownCommand = "shutdown";

	/*!
	 * \brief PComm Command. Used to indicate an error on the simulation's side.
	 * Outgoing:
	 * - int32_t Error Code
	 * - int32_t Error String Length
	 * - Error String
	 */
	static constexpr std::string_view ErrorCommand = "error";
};

/*!
 * \brief Simulation Server. User by NRP clients for communication with running experiments
 */
class SimulationServer
        : public SimulationServerConst
{
		/*!
		 * \brief Number of threads that handle incoming requests
		 */
		static constexpr uint8_t HandlingThreads = 5;

		/*!
		 * \brief Time (in ms) between packet retrieval attempts
		 */
		static constexpr useconds_t SleepTime = 200;

	public:
		/*!
		 * \brief Constructor. Starts the server asynchronously
		 * \param config Server configuration
		 * \param processLaunchers Process Launchers
		 * \param engineLaunchers Engine Launchers
		 * \param comm Pipe Communication
		 * \param commPID Communication partner PID
		 * \param confirmSignalOffsetNum Communication Confirmation Signal
		 */
		SimulationServer(const ServerConfigConstSharedPtr &config, MainProcessLauncherManagerConstSharedPtr &processLaunchers, EngineLauncherManagerSharedPtr &engineLaunchers,
		                 PipeCommunication &&comm, pid_t commPID, uint16_t confirmSignalOffsetNum = 0);

		/*!
		 * \brief Destructor. Stops handler threads, sends a shutdown message, then stops pipe communication
		 */
		~SimulationServer();

		/*!
		 * \brief Start the server asynchronously
		 */
		void startServerAsync();

		/*!
		 * \brief Shutdown the server
		 */
		void shutdownServer();

		/*!
		 * \brief Returns server status
		 */
		bool isServerRunning() const;

		/*!
		 * \brief Stop any currently running experiments and set config for new one
		 * \param simConfig Simulation Config Data. If nullptr, reuse old simulation config
		 */
		void resetSimulation(SimulationConfigSharedPtr simConfig = nullptr);

		/*!
		 * \brief Get Simulation Status
		 */
		nlohmann::json getSimStatus() const;

		/*!
		 * \brief Get simulation state
		 */
		SimulationStatus::SIM_STATE getSimState() const;

		/*!
		 * \brief Returns true if server is currently running running, false otherwise
		 */
		bool isSimRunning() const;

		/*!
		 * \brief Set simulation's running status. Will adjust _simRunningThread according to state
		 * \param state
		 * \return Returns true if state set was successfull, false otherwise
		 * (A state set might fail if no configuration is loaded or if the simulation loop was not initialized)
		 */
		bool setSimRunning(const SimulationRunningData &state);

	private:
		/*!
		 * \brief Threads that handle incoming requests
		 */
		std::array<std::thread, HandlingThreads> _threads;

		/*!
		 * \brief Thread in which to run simulation
		 */
		std::thread _simRunningThread;

		/*!
		 * \brief Pipe Communication
		 */
		PipePacketCommunication _comm;

		/*!
		 * \brief Server Config
		 */
		ServerConfigConstSharedPtr _config;

		/*!
		 * \brief Process Launchers
		 */
		MainProcessLauncherManagerConstSharedPtr _processLaunchers;

		/*!
		 * \brief Engine Launchers
		 */
		EngineLauncherManagerSharedPtr _engineLaunchers;

		/*!
		 * \brief Currently running simulation
		 */
		SimulationManagerSharedPtr _sim;

		using pack_handler_t = PipeCommPacket(SimulationServer::*)(const PipeCommPacket&);
		using handler_map_t = std::map<std::string, pack_handler_t>;

		/*!
		 * \brief Packet Handlers. Mapped from Command to Handler function
		 */
		handler_map_t _handlers;

		/*!
		 * \brief Function that processes incoming messages
		 */
		void handleThreadCallback();

		/*!
		 * \brief Setup handlers
		 */
		static handler_map_t setHandlers();

		// REST Route Handlers
		PipeCommPacket getSimStatusHandler(const PipeCommPacket &req);
		PipeCommPacket getSimRunningHandler(const PipeCommPacket &req);
		PipeCommPacket postSimRunningHandler(const PipeCommPacket &req);
		PipeCommPacket getEngineStatusHandler(const PipeCommPacket &req);
};

#endif // SIMULATION_SERVER_H
