//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

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
