#ifndef EXPERIMENT_MANAGER_H
#define EXPERIMENT_MANAGER_H

#include "nrp_simulation/config/cmake_conf.h"
#include "nrp_simulation/config/server_config.h"
#include "nrp_simulation/simulation/simulation_manager.h"
#include "nrp_simulation/utils/pipe_packet_communication.h"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

/*!
 * \brief Experiment Manager. Starts a server to manage experiments.
 */
class ExperimentManager
{
		using mutex_t = std::mutex;
		using lock_t = std::unique_lock<mutex_t>;

		struct RunningExperimentData
		{
			pid_t PID;
			PipePacketCommunication PComm;
		};

	public:
		static constexpr std::string_view GetRunningExperimentsRoute = "/get_running_experiments";
		static constexpr std::string_view GetServerStatusRoute       = "/get_server_status";
		static constexpr std::string_view UploadExperimentRoute      = "/upload_experiment";
		static constexpr std::string_view StartExperimentRoute       = "/start_experiment";

		/*!
		 * \brief Experiment Upload parameter name
		 */
		static constexpr std::string_view UploadExperimentNameParam = "exp_name";

		/*!
		 * \brief Constructor. Starts REST server
		 * \param config Configuration
		 */
		ExperimentManager(const ServerConfigConstSharedPtr &config);

		/*!
		 *	\brief Destructor. Stops REST server
		 */
		~ExperimentManager();

		/*!
		 * \brief Start the server asynchronously
		 */
		void startServerAsync();

		/*!
		 * \brief Shutdown the server. Experiments will keep running
		 */
		void shutdownServer();

		/*!
		 * \brief Returns server status
		 */
		bool isServerRunning() const;

		/*!
		 * \brief Acquire lock for experiment access
		 * \return Returns locked mutex
		 */
		lock_t acquireExperimentLock();

		/*!
		 * \brief Stop an experiment's process
		 * \param experimentName Name of experiment
		 * \param expLock Experiment Lock
		 */
		void shutdownExperiment(const std::string &experimentName, const lock_t &expLock);

	private:
		/*!
		 * \brief Configuration data
		 */
		ServerConfigConstSharedPtr _config;

		/*!
		 * \brief REST Routes
		 */
		Pistache::Rest::Router _router;

		/*!
		 * \brief REST Server
		 */
		Pistache::Http::Endpoint _server;

		/*!
		 * \brief Server Status Flag
		 */
		bool _serverRunning = false;

		/*!
		 * \brief Lock for _experiments access
		 */
		mutex_t _experimentLock;

		using exp_map_t = std::map<std::string, RunningExperimentData>;

		/*!
		 * \brief Experiment Data
		 */
		std::map<std::string, RunningExperimentData> _experiments;

		using pack_callback_fcn_t = std::function<void(PipeCommPacket&&)>;

		/*!
		 * \brief Callback for
		 */
		std::map<PipeCommPacket::packet_id_t, pack_callback_fcn_t> _callback;

		/*!
		 * \brief Stop an experiment's process
		 * \param expIt Iterator pointing to element in _experiments
		 * \param expLock Experiment Lock
		 */
		void shutdownExperiment(const exp_map_t::iterator &expIt, const lock_t &expLock);

		/*!
		 * \brief Setup Routes
		 */
		static Pistache::Rest::Router setupServerRoutes(ExperimentManager *expManager);

		void getRunningExperimentsHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);
		void getServerStatusHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);
		void uploadExperimentHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);
		void startExperimentHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);

		/*!
		 * \brief Kills experiment process and waits for it to quit
		 * \param data Experiment Data
		 */
		static void stopExperimentProcess(const RunningExperimentData &data);

		/*!
		 * \brief Get Username supplied by REST request. If HTTP Header Authorization is not set, will use ServerConfigConst::DefaultUsername
		 * \param req HTTP REST request
		 * \return Returns Username
		 */
		static std::string getUsername(const Pistache::Rest::Request &req);

		/*!
		 * \brief Get experiment name supplied by REST request.
		 * If parameter ExperimentManager::UploadExperimentNameParam not present, will use ServerConfigConst::DefaultExperimentName
		 * \param req HTTP REST request
		 * \return Returns Experiment name
		 */
		static std::string getExperimentName(const Pistache::Rest::Request &req);
};

#endif // EXPERIMENT_MANAGER_H
