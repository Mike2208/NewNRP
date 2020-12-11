/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef ENGINE_JSON_REGISTRATION_SERVER_H
#define ENGINE_JSON_REGISTRATION_SERVER_H

#include "nrp_general_library/device_interface/device.h"

#include <pistache/router.h>
#include <pistache/endpoint.h>

/*!
 *  \brief Singleton. Creates an HTTP REST server to register newly created EngineJSONServers and store their addresses
 */
class EngineJSONRegistrationServer
{
		/*!
		 * \brief Struct to handle REST calls
		 */
		struct RequestHandler : Pistache::Http::Handler
		{
			HTTP_PROTOTYPE(RequestHandler);

			RequestHandler(EngineJSONRegistrationServer *pServer);
			~RequestHandler() override;

			void onRequest(const Pistache::Http::Request& req, Pistache::Http::ResponseWriter response) override;

			private:
			    EngineJSONRegistrationServer *_pServer = nullptr;
		};

		using engine_name_t = decltype(DeviceIdentifier::EngineName);

	public:
		/*!
		 * \brief JSON Engine Name locator used during registration
		 */
		static constexpr std::string_view JSONEngineName = "engine_name";

		/*!
		 * \brief JSON Engine Address locator used during registration
		 */
		static constexpr std::string_view JSONAddress = "address";

		/*!
		 * \brief Get Instance of EngineJSONRegistrationServer
		 * \return Returns ptr to EngineJSONRegistrationServer if it exists, nullptr otherwise
		 */
		static EngineJSONRegistrationServer *getInstance();

		/*!
		 * \brief Reset EngineJSONRegistrationServer with the given address
		 * \param serverAddress Server Address to bind to
		 * \return Returns pointer to created instance
		 */
		static EngineJSONRegistrationServer *resetInstance(const std::string &serverAddress);

		/*!
		 * \brief Delete Instance
		 */
		static void clearInstance();

		// Delete copy and move operations for singleton
		EngineJSONRegistrationServer(const EngineJSONRegistrationServer&) = delete;
		EngineJSONRegistrationServer(EngineJSONRegistrationServer&&) = delete;

		EngineJSONRegistrationServer &operator=(const EngineJSONRegistrationServer&) = delete;
		EngineJSONRegistrationServer &operator=(EngineJSONRegistrationServer&&) = delete;

		~EngineJSONRegistrationServer();

		/*!
		 * \brief Start the server if it's not already running
		 */
		void startServerAsync();

		/*!
		 * \brief Stop the Server
		 */
		void shutdownServer();

		/*!
		 * \brief Get server address
		 */
		const std::string serverAddress() const;

		/*!
		 * \brief Returns true when server is running, false otherwise
		 */
		bool isRunning() const;

		/*!
		 * \brief Get the number of engines that are still waiting for registration
		 */
		size_t getNumWaitingEngines();

		/*!
		 * \brief Retrieve a registered engine address. If available and non-empty, erase it from _registeredAddresses
		 * \param engineName Engine Name for which to find the address
		 * \return If address available, return it. Otherwise return empty string
		 */
		std::string retrieveEngineAddress(const engine_name_t &engineName);

		/*!
		 * \brief Request an engine's address. If available, erases entry from _registeredAddresses
		 * \param engineName Name of engine to wait for
		 * \return If available, returns name of engine. Else, returns empty string
		 */
		std::string requestEngine(const engine_name_t &engineName);

		/*!
		 * \brief Register an engine's address
		 * \param engineName Name of engine
		 * \param address Address of engine
		 */
		void registerEngineAddress(const engine_name_t &engineName, const std::string &address);

		/*!
		 * \brief Send Engine Name and address to specified address
		 * \param address Address to send data to
		 * \param engineName Name of engine
		 * \param engineAddress Address of engine
		 * \param numTries Number of times to try and contact the registration server
		 * \param waitTime Time (in seconds) to wait between contact attempts
		 * \return Returns true on success, false otherwise
		 */
		static bool sendClientEngineRequest(const std::string &address, const engine_name_t &engineName, const std::string &engineAddress, const unsigned int numTries = 1, const unsigned int waitTime = 0);

	private:
		/*!
		 * \brief Server running state
		 */
		bool _serverRunning = false;

		/*!
		 * \brief Server address
		 */
		std::string _address;

		/*!
		 * \brief Server Endpoint
		 */
		Pistache::Http::Endpoint _endpoint;

		/*!
		 * \brief Prevent access to _registeredAddresses to allow thread-safety
		 */
		std::mutex _lock;

		/*!
		 * \brief Already registered addresses
		 */
		std::map<decltype(DeviceIdentifier::EngineName), std::string> _registeredAddresses;

		/*!
		 * \brief Only instance of this server
		 */
		static std::unique_ptr<EngineJSONRegistrationServer> _instance;

		/*!
		 * \brief Constructor
		 * \param address Address under which to make server accessible
		 */
		EngineJSONRegistrationServer(const std::string &address);

		friend struct EngineJSONRegistrationServer::RequestHandler;
};

#endif // ENGINE_JSON_REGISTRATION_SERVER_H
