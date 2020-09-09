#include "nrp_server/server_communication/server_communication.h"

#include <pistache/http.h>

using namespace Pistache;

ServerCommunication::ServerCommunication(const shared_const_server_config_t &serverComConfig)
    : _serverComConfig(serverComConfig),
      _serverEndpoint(std::make_shared<Http::Endpoint>(Address(serverComConfig->RESTBaseURL(), serverComConfig->RESTPort())))
{
	auto restOptions = Http::Endpoint::options().threads(serverComConfig->RESTThreads());
	this->_serverEndpoint->init(restOptions);
}

void ServerCommunication::addRoute(Http::Method method, const std::string &route, ServerCommunication::rest_fcn_t callbackFunction)
{
	this->_serverRouter.addRoute(method, route, callbackFunction);
}

void ServerCommunication::serveREST()
{
	this->_serverEndpoint->setHandler(this->_serverRouter.handler());

	this->_serverEndpoint->serve();
}
