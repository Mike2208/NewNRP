#include "nrp_world_plugin/nrp_world_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <gazebo/physics/PhysicsEngine.hh>
#include <gazebo/physics/World.hh>

void gazebo::NRPWorldPlugin::Load(gazebo::physics::WorldPtr world, sdf::ElementPtr sdf)
{
	std::cout << "NRPWorldPlugin: Loading world plugin...\n";

	this->_world = world;
	this->_worldSDF = sdf;

	// Pause simulation
	world->SetPaused(true);

	// Tell simulation to go as fast as possible
	world->Physics()->SetRealTimeUpdateRate(0);

	std::cout << "NRPWorldPlugin: Registering world controller with communicator...\n";
	NRPCommunicationController::getInstance().registerStepController(this);
}

void gazebo::NRPWorldPlugin::Reset()
{
	std::cout << "NRPWorldPlugin: Resetting world...\n";

	this->_world.reset();
	this->_worldSDF.reset();
}

SimulationTime gazebo::NRPWorldPlugin::runLoopStep(SimulationTime timeStep)
{
	std::scoped_lock lock(this->_lockLoop);

	//std::cout << "NRPWorldPlugin: Executing loop step\n";

	// Step simulation until timeStep seconds of simulated time have passed
	try
	{
		const double   timeStepDouble = static_cast<double>(timeStep.count());
		const unsigned numIterations  = std::max(static_cast<unsigned int>(timeStepDouble/this->_world->Physics()->GetMaxStepSize()), 1u);
		this->startLoop(numIterations);
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Error while executing gazebo step");
	}

	//std::cout << "NRPWorldPlugin: Finished loop step. Time:" <<  this->_world->SimTime().Double() << "\n";

	const SimulationTime simTimeS  = std::chrono::duration_cast<SimulationTime>(std::chrono::seconds    (this->_world->SimTime().sec ));
	const SimulationTime simTimeNs = std::chrono::duration_cast<SimulationTime>(std::chrono::nanoseconds(this->_world->SimTime().nsec));

	return simTimeS + simTimeNs;
}

bool gazebo::NRPWorldPlugin::finishWorldLoading()
{
	//std::cout << "Finalizing gazebo loading... Time:" <<  this->_world->SimTime().Double() << "\n";

	// Force loading of all plugins
	const auto prevStepSize = this->_world->Physics()->GetMaxStepSize();
	this->_world->Physics()->SetMaxStepSize(0);
	this->startLoop(1);
	this->_world->Physics()->SetMaxStepSize(prevStepSize);

	//std::cout << "Gazebo loading finalized Time:" <<  this->_world->SimTime().Double() << "\n";

	return true;
}

void gazebo::NRPWorldPlugin::startLoop(unsigned int numIterations)
{
	//std::cout << "NRPWorldPlugin: Running " << numIterations << " iterations\n";

	this->_world->Step(numIterations);
	this->_world->SetPaused(true);
}
