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

float NRPCommunicationController::runLoopStep(float timeStep)
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
		return this->_stepController->runLoopStep(static_cast<double>(timeStep));
	}
	catch(const std::exception &e)
	{
		std::cerr << "Error during Gazebo stepping\n";
		std::cerr << e.what();

		throw;
	}
}

json NRPCommunicationController::initialize(const json &data, EngineGrpcServer::lock_t &lock)
{
	ConfigStorage confDat(data);
	GazeboConfig conf(confDat);

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
    : EngineGrpcServer(address)
{}

NRPCommunicationController::NRPCommunicationController(const std::string &serverURL, const std::string &engineName, const std::string &registrationURL)
    : EngineGrpcServer(serverURL, engineName, registrationURL)
{}
