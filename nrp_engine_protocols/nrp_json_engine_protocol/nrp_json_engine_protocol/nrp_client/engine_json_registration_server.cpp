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

#include "nrp_json_engine_protocol/nrp_client/engine_json_registration_server.h"

#include "nrp_json_engine_protocol/config/engine_json_config.h"

#include <nlohmann/json.hpp>
#include <restclient-cpp/restclient.h>

std::unique_ptr<EngineJSONRegistrationServer> EngineJSONRegistrationServer::_instance = nullptr;

EngineJSONRegistrationServer::RequestHandler::RequestHandler(EngineJSONRegistrationServer *pServer)
    : _pServer(pServer)
{}

EngineJSONRegistrationServer::RequestHandler::~RequestHandler()
{
	this->_pServer = nullptr;
}

void EngineJSONRegistrationServer::RequestHandler::onRequest(const Pistache::Http::Request &req, Pistache::Http::ResponseWriter response)
{
	try
	{
		nlohmann::json data = nlohmann::json::parse(req.body());
		this->_pServer->registerEngineAddress(data[EngineJSONRegistrationServer::JSONEngineName.data()],
		                                      data[EngineJSONRegistrationServer::JSONAddress.data()]);
	}
	catch(std::exception &e)
	{
		const auto errMsg = std::string("Error while handling engine registration request: ") + e.what();
		const auto exc = NRPException::logCreate(e, errMsg);
		response.send(Pistache::Http::Code::Bad_Request, errMsg);
	}

	response.send(Pistache::Http::Code::Ok);
}

EngineJSONRegistrationServer *EngineJSONRegistrationServer::getInstance()
{
	return EngineJSONRegistrationServer::_instance.get();
}

EngineJSONRegistrationServer *EngineJSONRegistrationServer::resetInstance(const std::string &serverAddress)
{
	EngineJSONRegistrationServer::_instance.reset(new EngineJSONRegistrationServer(serverAddress));
	return EngineJSONRegistrationServer::getInstance();
}

void EngineJSONRegistrationServer::clearInstance()
{
	EngineJSONRegistrationServer::_instance.reset();
}

EngineJSONRegistrationServer::~EngineJSONRegistrationServer()
{
	this->shutdownServer();
}

void EngineJSONRegistrationServer::startServerAsync()
{
	if(!this->_serverRunning)
	{
		this->_endpoint.serveThreaded();
		this->_serverRunning = true;
	}
}

void EngineJSONRegistrationServer::shutdownServer()
{
	if(this->_serverRunning)
	{
		this->_serverRunning = false;
		this->_endpoint.shutdown();
	}
}

const std::string EngineJSONRegistrationServer::serverAddress() const
{
	return this->_address;
}

bool EngineJSONRegistrationServer::isRunning() const
{
	return this->_serverRunning;
}

size_t EngineJSONRegistrationServer::getNumWaitingEngines()
{
	std::scoped_lock lock(this->_lock);
	return this->_registeredAddresses.size();
}

std::string EngineJSONRegistrationServer::retrieveEngineAddress(const engine_name_t &engineName)
{
	std::scoped_lock lock(this->_lock);

	auto addressIterator = this->_registeredAddresses.find(engineName);
	if(addressIterator != this->_registeredAddresses.end())
	{
		// Find address, remove it from registeredAddresses
		const auto engineAddress = std::move(addressIterator->second);
		if(!engineAddress.empty())
			this->_registeredAddresses.erase(addressIterator);

		return engineAddress;
	}

	// Return empty string if engineName not found
	return "";
}

std::string EngineJSONRegistrationServer::requestEngine(const engine_name_t &engineName)
{
	{
		std::scoped_lock lock(this->_lock);
		this->_registeredAddresses.emplace(engineName, "");
	}

	return this->retrieveEngineAddress(engineName);
}

void EngineJSONRegistrationServer::registerEngineAddress(const engine_name_t &engineName, const std::string &address)
{
	std::scoped_lock lock(this->_lock);
	this->_registeredAddresses[engineName] = address;
}

bool EngineJSONRegistrationServer::sendClientEngineRequest(const std::string &address, const EngineJSONRegistrationServer::engine_name_t &engineName, const std::string &engineAddress, const unsigned int numTries, const unsigned int waitTime)
{
	const nlohmann::json data({ { JSONEngineName, engineName }, { JSONAddress, engineAddress} });

	bool res;
	unsigned int curTry = 0;

	// Try to register engine address numTries times
	do
	{
		auto resp = RestClient::post(address, EngineJSONConfigConst::EngineServerContentType.data(), data.dump());
		res = (resp.code == static_cast<int>(Pistache::Http::Code::Ok));
		if(res)
			return res;

		// Wait between attempts
		sleep(waitTime);
		++curTry;
	}
	while(curTry < numTries);

	return false;
}

EngineJSONRegistrationServer::EngineJSONRegistrationServer(const std::string &address)
    : _address(address),
      _endpoint(Pistache::Address(address))
{
	// Set Endpoint options
	auto options = Pistache::Http::Endpoint::options();
	options.threads(1).backlog(10);

	this->_endpoint.init(options);

	this->_endpoint.setHandler(Pistache::Http::make_handler<RequestHandler>(this));
}

