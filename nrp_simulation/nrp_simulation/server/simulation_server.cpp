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

#include "nrp_simulation/server/simulation_server.h"

#include <assert.h>
#include <iostream>
#include <unistd.h>


SimulationServer::SimulationServer(const ServerConfigConstSharedPtr &config, MainProcessLauncherManagerConstSharedPtr &processLaunchers, EngineLauncherManagerSharedPtr &engineLaunchers,
                                   PipeCommunication &&comm, pid_t commPID, uint16_t confirmSignalOffsetNum)
    : _comm(PipePacketCommunication(std::move(comm), commPID, confirmSignalOffsetNum)),
      _config(config),
      _processLaunchers(processLaunchers),
      _engineLaunchers(engineLaunchers),
      _handlers(SimulationServer::setHandlers())
{
	this->startServerAsync();
}

SimulationServer::~SimulationServer()
{
	this->shutdownServer();

	this->_sim = nullptr;

	PipeCommPacket packet;
	packet.ID = -1;
	packet.Command = SimulationServer::ShutdownInfoCommand;
	packet.Data.clear();

	this->_comm.sendPacketImmediately(std::move(packet));
}

void SimulationServer::startServerAsync()
{
	this->_comm.startServerAsync();

	for(auto &curThread : this->_threads)
	{
		// Make sure threads are not running
		assert(!curThread.joinable());

		curThread = std::thread(&SimulationServer::handleThreadCallback, this);
	}
}

void SimulationServer::shutdownServer()
{
	this->_comm.shutdownServer();

	// Make sure all threads have completed
	this->_simRunningThread.join();
	for(auto &curThread : this->_threads)
		curThread.join();
}

bool SimulationServer::isServerRunning() const
{
	return this->_comm.isRunning();
}

void SimulationServer::resetSimulation(SimulationConfigSharedPtr simConfig)
{
	{
		SimulationManager::sim_lock_t lock;
		if(simConfig == nullptr)
		{
			if(this->_sim != nullptr)
				simConfig = this->_sim->simulationConfig(lock);
			else
				simConfig = nullptr;
		}

		this->_sim->shutdownLoop(lock);
	}

	this->_sim.reset(new SimulationManager(this->_config, simConfig));
}

nlohmann::json SimulationServer::getSimStatus() const
{
	using SIM_STATE = SimulationStatus::SIM_STATE;

	const auto simState = this->getSimState();
	const auto simTime = simState >= SIM_STATE::SIMULATION_LOADED ? this->_sim->simulationLoop()->getSimTime() : 0.f;
	const auto &simConf = const_cast<const SimulationManager&>(*this->_sim).simulationConfig();

	return SimulationStatus(simState, simTime, *simConf).serializeProperties(nlohmann::json());
}

SimulationStatusConst::SIM_STATE SimulationServer::getSimState() const
{
	using SIM_STATE = SimulationStatus::SIM_STATE;
	// First, perform checks that don't require locking the simulation

	// Check if simulation is initializing
	if(this->_sim->isSimInitializing())
		return SIM_STATE::ENGINE_SETUP;

	// Check if simulation is running
	if(this->_sim->isRunning())
		return SIM_STATE::RUNNING;

	// Now, check rest of states
	SimulationManager::sim_lock_t lock = this->_sim->acquireSimLock();

	if(this->_sim->simulationConfig(lock) == nullptr)
		return SIM_STATE::STARTUP;
	else if(this->_sim->simulationLoop() == nullptr)
		return SIM_STATE::CONFIG_LOADED;
	else if(this->_sim->isRunning())
		return SIM_STATE::RUNNING;
	else if(this->_sim->simulationLoop()->getSimTime() <= 0.f)
		return SIM_STATE::SIMULATION_LOADED;
}

bool SimulationServer::isSimRunning() const
{
	using SIM_STATE = SimulationStatus::SIM_STATE;

	return this->getSimState() == SIM_STATE::RUNNING;
}

bool SimulationServer::setSimRunning(const SimulationRunningData &state)
{
	// Cannot set running state if simulation has not yet been initialized
	using SIM_STATE = SimulationStatus::SIM_STATE;
	if(this->getSimState() < SIM_STATE::SIMULATION_LOADED)
		return false;

	// Set simulation timeout
	if(state.TimeOut != SimulationRunningData::IgnoreTimeout && state.TimeOut >= 0)
	{
		auto lock = this->_sim->acquireSimLock();
		this->_sim->simulationConfig(lock)->simulationTimeOut() = state.TimeOut;
	}

	if(!state.SetRunning)
	{
		// Pause simulation if currently running
		if(this->_simRunningThread.joinable())
		{
			// Wait for current timeStep to complete
			this->_sim->stopSimulation(this->_sim->acquireSimLock());
			this->_simRunningThread.join();
		}
	}
	else
	{
		// Check whether simulation is already running
		if(!this->_simRunningThread.joinable())
		{
			auto simLock = this->_sim->acquireSimLock();
			auto *simManager = this->_sim.get();

			// Start running thread in a way that passes lock
			this->_simRunningThread = std::thread([simManager](SimulationManager::sim_lock_t &&lock) { simManager->runSimulationUntilTimeout(lock); }, std::move(simLock));
		}
	}
}

void SimulationServer::handleThreadCallback()
{
	while(this->isServerRunning())
	{
		const auto curPacket = this->_comm.retrievePacket();

		// Check if a packet was received
		if(curPacket.ID <= 0)
		{
			usleep(SimulationServer::SleepTime);
			continue;
		}

		// Check that command has an associated handler
		const auto handlersIt = this->_handlers.find(curPacket.Command);
		if(handlersIt == this->_handlers.end())
		{
			std::cerr << "Received unknown packet command: " << curPacket.Command << std::endl;
			continue;
		}

		// Handle packet
		PipeCommPacket retPacket;
		try
		{
			retPacket = std::invoke(handlersIt->second, this, curPacket);
		}
		catch(std::exception &e)
		{
			// In case of error, inform parent
			retPacket.ID = curPacket.ID;
			retPacket.Command = SimulationServer::ErrorCommand;

			const std::string errMsg = std::string("Failed to process package: ") + e.what();
			std::cerr << errMsg << std::endl;

			retPacket.Data.resize(errMsg.size()+1);
			memcpy(retPacket.Data.data(), errMsg.data(), errMsg.size()+1);

			this->_comm.shutdownServer();

			if(this->_comm.sendPacketImmediately(std::move(retPacket)) < 0)
				std::cerr << "Failed to inform parent process of failure!";

			// Escalate exception
			throw;
		}

		this->_comm.sendPacket(std::move(retPacket));
	}
}

SimulationServer::handler_map_t SimulationServer::setHandlers()
{
	handler_map_t handlers;
	handlers.clear();

	handlers.emplace(GetSimStatusCommand.data(), &SimulationServer::getSimStatusHandler);
	handlers.emplace(GetSimRunningCommand.data(), &SimulationServer::getSimRunningHandler);
	handlers.emplace(PostSimRunningCommand.data(), &SimulationServer::postSimRunningHandler);
	handlers.emplace(GetEngineDataCommand.data(), &SimulationServer::getEngineStatusHandler);

	return handlers;
}

PipeCommPacket SimulationServer::getSimStatusHandler(const PipeCommPacket &req)
{
	const auto status = this->getSimStatus().dump();

	PipeCommPacket retPack;
	retPack.ID = req.ID;
	retPack.Command = SimulationServer::ReturnCommand;

	retPack.Data.resize(status.size()+1);
	memcpy(retPack.Data.data(), status.data(), status.size()+1);

	return retPack;
}

PipeCommPacket SimulationServer::getSimRunningHandler(const PipeCommPacket &req)
{
	const auto isRunning = this->isSimRunning();

	PipeCommPacket retPack;
	retPack.ID = req.ID;
	retPack.Command = SimulationServer::ReturnCommand;

	retPack.Data.resize(sizeof(isRunning));
	memcpy(retPack.Data.data(), &isRunning, sizeof(isRunning));

	return retPack;
}
