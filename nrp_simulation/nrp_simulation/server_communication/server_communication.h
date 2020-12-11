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

#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include "nrp_simulation/config/server_communication_config.h"

#include <memory>
#include <pistache/router.h>
#include <pistache/endpoint.h>

/*!
 * \brief Manages server communications over REST
 */
class ServerCommunication
{
		using shared_const_server_config_t = std::shared_ptr<const ServerCommunicationConfig>;

	public:
		using rest_fcn_t = std::function<Pistache::Rest::Route::Result(const Pistache::Http::Request, Pistache::Http::ResponseWriter)>;

		/*!
		 * \brief Startup REST server using the given config options
		 * \param serverConfig
		 */
		ServerCommunication(const shared_const_server_config_t &serverComConfig);

		/*!
		 * \brief Add Route to REST server
		 */
		void addRoute(Pistache::Http::Method method, const std::string &route, rest_fcn_t callbackFunction);

		/*!
		 * \brief Starts the REST server
		 */
		void serveREST();

	private:
		/*!
		 * \brief Server CommunicationConfiguration
		 */
		shared_const_server_config_t _serverComConfig;

		/*!
		 * \brief Manager of all REST routes
		 */
		Pistache::Rest::Router _serverRouter;

		/*!
		 * \brief REST Endpoint
		 */
		std::shared_ptr<Pistache::Http::Endpoint> _serverEndpoint;
};

#endif // SERVER_COMMUNICATION_H
