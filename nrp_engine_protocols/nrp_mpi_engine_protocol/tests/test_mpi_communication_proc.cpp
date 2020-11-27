#include "nrp_general_library/utils/mpi_communication.h"
#include "nrp_general_library/utils/mpi_setup.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "tests/test_env_cmake.h"

int main(int argc, char **argv)
{
	// Setup python (Used by MPI4Py)
	PythonInterpreterState pyState(argc, argv);

	// Initialize MPI
	std::cout << "MPI Child process: Initializing MPI..." << std::endl;
	MPISetup::initializeOnce(argc, argv);
	std::cout << "MPI Child process: Initialized MPI" << std::endl;
	MPI_Comm parentComm = MPISetup::getParentComm();

	// Send PID
	std::cout << "MPI Child process: Sending PID..." << std::endl;
	MPISetup::getInstance()->sendPID(parentComm);

	// Send string
	std::cout << "MPI Child process: Sending string..." << std::endl;
	MPICommunication::sendString(parentComm, 0, TEST_MPI_COMM_STR);

	// Send device ID
	std::cout << "MPI Child process: Sending Device ID..." << std::endl;
	const DeviceIdentifier devID = TEST_MPI_COMM_DEVID;
	MPICommunication::sendDeviceID(parentComm, 0, devID);

	// Send device
	std::cout << "MPI Child process: Sending Device TestDevice1..." << std::endl;
	TestDevice1 tDev1 = TEST_MPI_COMM_DEV1;
	MPICommunication::sendDevice<TestDevice1, true>(parentComm, 0, tDev1);

	MPISetup::finalize();

	return 0;
}
