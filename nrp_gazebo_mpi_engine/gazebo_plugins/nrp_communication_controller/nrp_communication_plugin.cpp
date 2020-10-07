#include "nrp_communication_controller/nrp_communication_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"
#include "nrp_general_library/utils/mpi_setup.h"

void gazebo::NRPCommunicationPlugin::Load(int argc, char **argv)
{
	std::cout << "NRP Communication plugin: Initializing...\n";

	MPISetup::initializeOnce(argc, argv);

	// Reset communication controller
	NRPCommunicationController::resetInstance();

	std::cout << "NRP Communication plugin: MPI Initialized started. Waiting for input...\n";
}

void gazebo::NRPCommunicationPlugin::Reset()
{
	// Reset server
	std::cout << "NRP Communication plugin: Resetting controller...\n";
	NRPCommunicationController::resetInstance();
	std::cout << "NRP Communication plugin: Waiting for input...\n";
}
