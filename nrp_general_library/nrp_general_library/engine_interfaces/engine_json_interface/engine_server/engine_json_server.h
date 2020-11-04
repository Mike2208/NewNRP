#ifndef ENGINE_JSON_SERVER_H
#define ENGINE_JSON_SERVER_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"

#include <map>
#include <memory>
#include <pistache/router.h>
#include <pistache/endpoint.h>

/*! \addtogroup engine_types
\ref json_engine: An engine which relies on an HTTP REST server for communication. Serializes devices into JSON format
 */

/*! \defgroup json_engine REST JSON Engine
The REST JSON engine uses an HTTP REST Server as the base for the Engine server. The client can then use REST calls to communicate.
All communication will be de-/serialized using JSON.

The server is defined in EngineJSONServer, the configuration in EngineJSONConfig, and the client in EngineJSONNRPClient.

To control devices, EngineJSONDeviceController can be registered with the server.

To help create servers, EngineJSONOptsParser can be used to parse start parameters and extract relevant information.
Additionally, the CLE will launch an instance of EngineJSONRegistrationServer which can be used by EngineJSONServers communicate its address with clients.

TODO: Remove JSONDeviceConversionMechanism and only use JSONPropertySerializer.

TODO: Rewrite EngineJSONDeviceController to use classes as inputs/outputs instead of json.
 */

/*!
 *  \brief Manages communication with the NRP. Uses a REST server to send/receive data. Singleton class.
 */
class EngineJSONServer
{
		/*!
		 * \brief Time to wait for shutdown before raising an exception (in milliseconds)
		 */
		static constexpr auto ShutdownWaitTime = std::chrono::milliseconds(10*1000);

	protected:
		static constexpr std::string_view GetDeviceInformationRoute = EngineJSONConfigConst::EngineServerGetDevicesRoute;
		static constexpr std::string_view SetDeviceRoute = EngineJSONConfigConst::EngineServerSetDevicesRoute;
		static constexpr std::string_view RunLoopStepRoute = EngineJSONConfigConst::EngineServerRunLoopStepRoute;
		static constexpr std::string_view InitializeRoute = EngineJSONConfigConst::EngineServerInitializeRoute;
		static constexpr std::string_view ShutdownRoute = EngineJSONConfigConst::EngineServerShutdownRoute;

		using dcm_t = DeviceConversionMechanism<nlohmann::json, nlohmann::json::const_iterator>;

	public:
		using mutex_t = std::timed_mutex;
		using lock_t = std::unique_lock<EngineJSONServer::mutex_t>;

		/*!
		 * \brief Constructor. Tries to bind to a port and register itself with clientAddress
		 * \param engineAddress Server Address. If it contains a port, will try to bind to said port. If that fails, will increment port number and try again. This will continue for at most EngineJSONConfigConst::MaxAddrBindTries times
		 * \param engineName Engine Name
		 * \param clientAddress Client Address. The server will try to register itself under this address
		 */
		EngineJSONServer(const std::string &engineAddress, const std::string &engineName, const std::string &clientAddress);

		/*!
		 * \brief Constructor. Will try to bind to engineAddress
		 * \param engineAddress Server address
		 */
		EngineJSONServer(const std::string &engineAddress);
		EngineJSONServer();
		virtual ~EngineJSONServer();

		// Delete copy mechanisms
		EngineJSONServer(const EngineJSONServer&) = delete;
		EngineJSONServer &operator=(const EngineJSONServer&) = delete;


		/*!
		 * \brief Is the server running?
		 * \return Returns true if the server is running
		 */
		bool isServerRunning() const;

		/*!
		 * \brief Start the server in asynchronous mode
		 */
		void startServerAsync();

		/*!
		 * \brief Start the server synchronoulsy
		 */
		void startServer();

		/*!
		 * \brief Stop running server
		 */
		void shutdownServer();

		/*!
		 * \brief Get running server port
		 * \return Returns port of running server, 0 if server is not running
		 */
		uint16_t serverPort() const;

		/*!
		 * \brief Get server address
		 */
		std::string serverAddress() const;

		/*!
		 * \brief Registers a device
		 * \param deviceName Name of device
		 * \param interface Pointer to interface
		 */
		void registerDevice(const std::string &deviceName, EngineJSONDeviceController *interface);

		/*!
		 * \brief Registers a device. Skips locking the mutex.
		 * Should only be used if thread-safe access to _devicesControllers can be guaranteed
		 * \param deviceName Name of device
		 * \param interface Pointer to interface
		 */
		void registerDeviceNoLock(const std::string &deviceName, EngineJSONDeviceController *interface);

		/*!
		 * \brief Run a single loop step
		 * \param timeStep Step to take
		 * \return Returns the time registered by this engine at the end of the loop
		 */
		virtual float runLoopStep(float timeStep) = 0;

		/*!
		 * \brief Engine Initialization routine
		 * \param data Initialization data
		 * \param deviceLock Device Lock. Prevents access to _devicesControllers
		 * \return Returns data about initialization status
		 */
		virtual nlohmann::json initialize(const nlohmann::json &data, EngineJSONServer::lock_t &deviceLock) = 0;

		/*!
		 * \brief Engine Shutdown routine
		 * \param data Shutdown data
		 * \return Returns data about shutdown status
		 */
		virtual nlohmann::json shutdown(const nlohmann::json &data) = 0;

	protected:
		/*!
		 * \brief Lock access to _devices to make execution thread-safe
		 */
		mutex_t _deviceLock;

		/*!
		 * \brief Remove all registered devices
		 */
		void clearRegisteredDevices();


		/*!
		 * \brief Retrieves device data. Takes an array of device names for which to get data.
		 * \param reqData A JSON array containing device names.
		 * \return Device data, formatted as a JSON array
		 */
		virtual nlohmann::json getDeviceData(const nlohmann::json &reqData);

		/*!
		 * \brief Set device data
		 * \param reqData A JSON array containing device names linked to the individual device's data
		 * \return Execution result
		 */
		virtual nlohmann::json setDeviceData(const nlohmann::json &reqData);

	private:
		/*!
		 * \brief Is the server running?
		 */
		bool _serverRunning = false;

		/*!
		 *	\brief ServerURL
		 */
		std::string _serverAddress;

		/*!
		 * \brief Routes
		 */
		Pistache::Rest::Router _router;

		using enpoint_ptr_t = std::unique_ptr<Pistache::Http::Endpoint>;

		/*!
		 * \brief Server Endpoint
		 */
		enpoint_ptr_t _pEndpoint = nullptr;

		/*!
		 * \brief Available devices
		 */
		std::map<std::string, EngineJSONDeviceController*> _devicesControllers;

		/*!
		 * \brief Set routes used by server
		 */
		static Pistache::Rest::Router setRoutes(EngineJSONServer *server);

		/*!
		 * \brief Parse request
		 * \param req Request
		 * \paragraph res Response Writer. Sends back failure message if parse failed
		 * \return Returns JSON object
		 */
		static nlohmann::json parseRequest(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter &res);

		/*!
		 * \brief Callback function to retrieve device data. Takes an array of device names for which to get data.
		 * \param req Device data request. A JSON array containing device names.
		 * \param res Response writer that sends device data back to NRP, formatted as a JSON array.
		 */
		void getDeviceDataHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);

		/*!
		 * \brief Callback function to set device data
		 * \param req Device Data. A JSON array containing device names linked to the individual device's data
		 * \param res Response writer that sends execution result back to the NRP
		 */
		void setDeviceHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);

		/*!
		 * \brief Try to get iterator key, print error message and throw exception if no key is available
		 * \param jsonIterator JSON object iterator
		 * \return Returns key
		 */
		static const std::string &getIteratorKey(const nlohmann::json::const_iterator &jsonIterator);

		/*!
		 * \brief Callback function to run loop step
		 * \param req Loop Data. Only contains the simulation time to be executed (in s)
		 * \param res Response writer that sends OK on completion
		 */
		void runLoopStepHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);

		/*!
		 * \brief Callback function to initialize
		 * \param req Initialization Data
		 * \param res Response writer. Contains initialization status
		 */
		void initializeHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);

		/*!
		 * \brief Callback function for shutdown routine
		 * \param req Shutdown Data
		 * \param res Response writer. Contains shutdown data
		 */
		void shutdownHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res);

		/*!
		 * \brief Creates a REST server. Tries to bind to a port and register itself with clientAddress
		 * \param engineAddress Server Address. If it contains a port, will try to bind to said port. If that fails, will increment port number and try again. This will continue for at most EngineJSONConfigConst::MaxAddrBindTries times
		 * \param engineName Engine Name
		 * \param clientAddress Client Address. The server will try to register itself under this address
		 */
		static Pistache::Http::Endpoint createEndpoint(std::string *engineAddress, const std::string &engineName);

		friend class EngineJSONServerTest_Functions_Test;
		friend class TestEngineJSONServer;
};

#endif // ENGINE_JSON_SERVER_H
