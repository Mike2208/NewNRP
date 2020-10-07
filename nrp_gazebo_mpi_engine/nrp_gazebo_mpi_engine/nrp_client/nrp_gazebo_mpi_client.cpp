#include "nrp_gazebo_mpi_engine/nrp_client/nrp_gazebo_mpi_client.h"

#include "nrp_general_library/process_launchers/launch_commands/mpi_spawn.h"

#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>

NRPGazeboMPIClient::NRPGazeboMPIClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
    : NRPMPIClient(config, std::move(launcher))
{}

EngineInterface::RESULT NRPGazeboMPIClient::initialize()
{
	auto confDat = this->engineConfig()->writeConfig();
	MPICommunication::sendString(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, confDat.dump());

	return EngineInterface::SUCCESS;
}

EngineInterface::RESULT NRPGazeboMPIClient::shutdown()
{
	return EngineInterface::SUCCESS;
}
