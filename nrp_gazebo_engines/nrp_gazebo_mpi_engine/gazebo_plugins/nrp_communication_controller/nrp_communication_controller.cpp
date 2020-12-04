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

void NRPCommunicationController::runLoopStep(float timeStep)
{
	if(this->_stepController == nullptr)
		throw NRPException::logCreate("Tried to run loop while the controller has not yet been initialized");

	try
	{
		// Execute loop step (Note: The _deviceLock mutex has already been set by EngineJSONServer::runLoopStepHandler, so no calls to reading/writing from/to devices is possible at this moment)
		this->_stepController->runLoopStep(static_cast<double>(timeStep));
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Error during Gazebo stepping");
	}
}

float NRPCommunicationController::getSimTime() const
{
	if(this->_stepController == nullptr)
		throw NRPException::logCreate("Tried to run loop while the controller has not yet been initialized");

	return this->_stepController->getSimTime();
}

NRPCommunicationController::NRPCommunicationController(MPI_Comm nrpComm)
    : EngineMPIServer(nrpComm)
{}
