#include "nrp_communication_controller/nrp_communication_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"
#include "nrp_general_library/utils/mpi_setup.h"

void gazebo::NRPCommunicationPlugin::Load(int argc, char **argv)
{
	std::cout << "NRP Communication plugin: Initializing...\n";

	this->_pyState = PythonInterpreterState();
	MPISetup::initializeOnce(0, nullptr, true);

	// Reset communication controller
	NRPCommunicationController::resetInstance();

	std::cout << "NRP Communication plugin: MPI Initialized started. Waiting for input...\n";
	this->_commThread = std::async(std::launch::async, &NRPCommunicationPlugin::commThreadFcn);
}

void gazebo::NRPCommunicationPlugin::Reset()
{
	// Reset server
	std::cout << "NRP Communication plugin: Resetting controller...\n";
	NRPCommunicationController::resetInstance();
	std::cout << "NRP Communication plugin: Waiting for input...\n";
}

std::string gazebo::NRPCommunicationPlugin::commThreadFcn() noexcept
{
	auto &mpiComm = NRPCommunicationController::getInstance();
	EngineMPIControl controlCmd(EngineMPIControlConst::NONE, 0l);
	do
	{
		try
		{
			mpiComm.getClientCmd(controlCmd);
			mpiComm.handleClientCmd(controlCmd);
		}
		catch(std::exception &e)
		{
			return NRPException::logCreate(e, "MPI Communication Failure").what();
		}
	}
	while(mpiComm.getEngineState() != EngineMPIServer::STOPPED);

	return "";
}
