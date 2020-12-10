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
