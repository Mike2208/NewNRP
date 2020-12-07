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
	EngineJSONServer::lock_t lock(this->_deviceLock);
	this->_stepController = stepController;
}

SimulationTime NRPCommunicationController::runLoopStep(SimulationTime timeStep)
{
	if(this->_stepController == nullptr)
		throw NRPException::logCreate("Tried to run loop while the controller has not yet been initialized");

	try
	{
		// Execute loop step (Note: The _deviceLock mutex has already been set by EngineJSONServer::runLoopStepHandler, so no calls to reading/writing from/to devices is possible at this moment)
		return this->_stepController->runLoopStep(timeStep);
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Error during Gazebo stepping");
	}
}

json NRPCommunicationController::initialize(const json &data, EngineJSONServer::lock_t &lock)
{
	ConfigStorage confDat(data);
	GazeboJSONConfig conf(confDat);

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
			return nlohmann::json({false});
		}
	}

	lock.lock();

	return nlohmann::json({true});
}

json NRPCommunicationController::shutdown(const json&)
{
	return nlohmann::json();
}

NRPCommunicationController::NRPCommunicationController(const std::string &address)
    : EngineJSONServer(address)
{}

NRPCommunicationController::NRPCommunicationController(const std::string &serverURL, const std::string &engineName, const std::string &registrationURL)
    : EngineJSONServer(serverURL, engineName, registrationURL)
{}
