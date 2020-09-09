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
