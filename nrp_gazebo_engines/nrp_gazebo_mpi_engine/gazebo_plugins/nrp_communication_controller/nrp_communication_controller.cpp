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

#include "nrp_general_library/utils/nrp_exceptions.h"

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

NRPCommunicationController &NRPCommunicationController::resetInstance(MPI_Comm nrpComm)
{
	// Remove old server, start new one with given server URL
	NRPCommunicationController::_instance.reset(new NRPCommunicationController(nrpComm));
	return NRPCommunicationController::getInstance();
}

void NRPCommunicationController::registerStepController(GazeboStepController *stepController)
{
	this->_stepController = stepController;
}

void NRPCommunicationController::initialize(const std::string &initData)
{
	ConfigStorage confDat;

	try
	{
		confDat.Data = nlohmann::json::parse(initData);
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Unable to parse initialization data");
	}

	GazeboConfig conf(confDat);

	double waitTime = conf.maxWorldLoadTime();
	if(conf.maxWorldLoadTime() <= 0)
		waitTime = std::numeric_limits<double>::max();

	// Wait until world plugin loads and forces a load of all other plugins
	while(this->_stepController == nullptr ? 1 : !this->_stepController->finishWorldLoading())
	{
		// Wait for 100ms before retrying
		waitTime -= 0.1;
		usleep(100*1000);

		if(waitTime <= 0)
			throw NRPException::logCreate("No Gazebo World StepController loaded. Unable to step simulation. Aborting...");
	}
}

void NRPCommunicationController::shutdown(const std::string &shutdownData)
{}

void NRPCommunicationController::runLoopStep(SimulationTime::zero() timeStep)
{
	if(this->_stepController == nullptr)
		throw NRPException::logCreate("Tried to run loop while the controller has not yet been initialized");

	try
	{
		// Execute loop step (Note: The _deviceLock mutex has already been set by EngineJSONServer::runLoopStepHandler, so no calls to reading/writing from/to devices is possible at this moment)
		this->_stepController->runLoopStep(timeStep);
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Error during Gazebo stepping");
	}
}

SimulationTime NRPCommunicationController::getSimTime() const
{
	if(this->_stepController == nullptr)
		throw NRPException::logCreate("Tried to run loop while the controller has not yet been initialized");

	return this->_stepController->getSimTime();
}

NRPCommunicationController::NRPCommunicationController(MPI_Comm nrpComm)
    : EngineMPIServer(nrpComm)
{}
