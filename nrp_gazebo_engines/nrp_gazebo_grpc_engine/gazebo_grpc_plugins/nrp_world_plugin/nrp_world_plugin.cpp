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

#include "nrp_world_plugin/nrp_world_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

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

	try
	{
		const auto     maxStepSizeUs = toSimulationTime<double, std::ratio<1>>(this->_world->Physics()->GetMaxStepSize());
		const unsigned numIterations = std::max(static_cast<unsigned int>(static_cast<double>(timeStep.count()) / static_cast<double>(maxStepSizeUs.count())), 1u);

		this->startLoop(numIterations);
	}
	catch(const std::exception &e)
	{
		std::cerr << "Error while executing gazebo step\n";
		std::cerr << e.what();

		throw;
	}

	//std::cout << "NRPWorldPlugin: Finished loop step. Time:" <<  this->_world->SimTime().Double() << "\n";

	const auto simTime = this->_world->SimTime();

	return toSimulationTime<int32_t, std::ratio<1>>(simTime.sec) + toSimulationTime<int32_t, std::nano>(simTime.nsec);
}

bool gazebo::NRPWorldPlugin::finishWorldLoading()
{
	std::cout << "Finalizing gazebo loading... Time:" <<  this->_world->SimTime().Double() << "\n";

	// Force loading of all plugins
	const auto prevStepSize = this->_world->Physics()->GetMaxStepSize();
	this->_world->Physics()->SetMaxStepSize(0);
	this->startLoop(1);
	this->_world->Physics()->SetMaxStepSize(prevStepSize);

	std::cout << "Gazebo loading finalized Time:" <<  this->_world->SimTime().Double() << "\n";

	return true;
}

void gazebo::NRPWorldPlugin::startLoop(unsigned int numIterations)
{
	//std::cout << "NRPWorldPlugin: Running " << numIterations << " iterations\n";

	this->_world->Step(numIterations);
	this->_world->SetPaused(true);
}
