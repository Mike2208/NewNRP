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

#include "nrp_communication_controller/nrp_communication_controller.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;

std::unique_ptr<NRPCommunicationController> NRPCommunicationController::_instance = nullptr;

NRPCommunicationController::~NRPCommunicationController()
{
	this->_stepController = nullptr;
}

NRPCommunicationController &NRPCommunicationController::getInstance()
{
	return *(NRPCommunicationController::_instance.get());
}

NRPCommunicationController &NRPCommunicationController::resetInstance(const std::string &serverURL)
{
	// Remove old server, start new one with given server URL
	NRPCommunicationController::_instance.reset(new NRPCommunicationController(serverURL));
	return NRPCommunicationController::getInstance();
}

NRPCommunicationController &NRPCommunicationController::resetInstance(const std::string &serverURL, const std::string &engineName, const std::string &registrationURL)
{
	NRPCommunicationController::_instance.reset(new NRPCommunicationController(serverURL, engineName, registrationURL));
	return NRPCommunicationController::getInstance();
}

void NRPCommunicationController::registerStepController(GazeboStepController *stepController)
{
	EngineGrpcServer::lock_t lock(this->_deviceLock);
	this->_stepController = stepController;
}

SimulationTime NRPCommunicationController::runLoopStep(SimulationTime timeStep)
{
	if(this->_stepController == nullptr)
	{
		auto err = std::out_of_range("Tried to run loop while the controller has not yet been initialized");
		std::cerr << err.what();

		throw err;
	}

	try
	{
		// Execute loop step (Note: The _deviceLock mutex has already been set by EngineJSONServer::runLoopStepHandler, so no calls to reading/writing from/to devices is possible at this moment)
		return this->_stepController->runLoopStep(timeStep);
	}
	catch(const std::exception &e)
	{
		std::cerr << "Error during Gazebo stepping\n";
		std::cerr << e.what();

		throw;
	}
}

void NRPCommunicationController::initialize(const json &data, EngineGrpcServer::lock_t &lock)
{
	ConfigStorage confDat(data);
	GazeboGrpcConfig conf(confDat);

	double waitTime = conf.maxWorldLoadTime();
	if(conf.maxWorldLoadTime() <= 0)
		waitTime = std::numeric_limits<double>::max();

	// Allow devices to register
	lock.unlock();

	// Wait until world plugin loads and forces a load of all other plugins
	while(this->_stepController == nullptr ? 1 : !this->_stepController->finishWorldLoading())
	{
		// Wait for 100ms before retrying
		waitTime -= 0.1;
		usleep(100*1000);

		if(waitTime <= 0)
		{
			lock.lock();

			const auto errMsg = "Couldn't load world";
            throw std::runtime_error(errMsg);
		}
	}

	lock.lock();
}

void NRPCommunicationController::shutdown(const json&)
{
	// Do nothing
}

NRPCommunicationController::NRPCommunicationController(const std::string &address)
    : EngineGrpcServer(address)
{}

NRPCommunicationController::NRPCommunicationController(const std::string &serverURL, const std::string &engineName, const std::string &registrationURL)
    : EngineGrpcServer(serverURL, engineName, registrationURL)
{}
