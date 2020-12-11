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

#include "nrp_general_library/utils/mpi_setup.h"

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"
#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer_methods.h"

// Include mpi before mpi4py
#include <mpi.h>
#include <mpi4py/mpi4py.MPI_api.h>

std::unique_ptr<MPISetup> MPISetup::_instance = nullptr;

MPISetup *MPISetup::getInstance()
{	return MPISetup::_instance.get();	}

MPISetup *MPISetup::resetInstance(int argc, char **argv)
{
	MPISetup::_instance.reset(new MPISetup(argc, argv));
	return MPISetup::getInstance();
}

MPISetup *MPISetup::initializeOnce(int argc, char **argv, bool sendPID)
{
	if(MPISetup::getInstance() == nullptr)
	{
		MPISetup *const retVal = MPISetup::resetInstance(argc, argv);

		if(sendPID)
		{
			MPI_Comm parentComm = MPISetup::getParentComm();
			if(parentComm == MPI_COMM_NULL)
				throw NRPException::logCreate("No parent PID intercommunicator found. Has this process been launched via MPI_Comm_spawn()?");

			retVal->sendPID(parentComm, 0);
		}

		return retVal;
	}

	return MPISetup::getInstance();
}

void MPISetup::finalize()
{	MPISetup::_instance.reset();	}

std::string MPISetup::getErrorString(int MPIErrorCode)
{
	int resLen = 1024;

	std::string retVal;
	retVal.resize(resLen);

	if(MPI_Error_string(MPIErrorCode, retVal.data(), &resLen) != 0)
		throw NRPException::logCreate("Failed to read MPI Error Code " + std::to_string(MPIErrorCode));

	return retVal;
}

MPI_Comm MPISetup::getParentComm()
{
	MPI_Comm retVal;

	const auto errc = MPI_Comm_get_parent(&retVal);
	if(errc != MPI_SUCCESS)
	{
		NRPLogger::SPDErrLogDefault("Failed to get parent proces MPI Comm: " + MPISetup::getErrorString(errc));

		retVal = MPI_COMM_NULL;
	}
	else
	{
		assert(retVal != MPI_COMM_NULL);
	}

	return retVal;
}

void MPISetup::sendPID(MPI_Comm comm, int tag)
{
	const pid_t ppid = getppid();
	MPICommunication::sendMPI(&ppid, sizeof(pid_t), MPI_BYTE, 0, tag, comm);
}

pid_t MPISetup::recvPID(MPI_Comm comm, int tag)
{
	pid_t retVal;
	MPICommunication::recvMPI(&retVal, sizeof(pid_t), MPI_BYTE, MPI_ANY_SOURCE, tag, comm);

	return retVal;
}

MPISetup::~MPISetup()
{
	const int errc = MPI_Finalize();
	if(errc != 0)
	{
		std::string mpiErrMsg;
		try
		{
			mpiErrMsg = MPISetup::getErrorString(errc);
		}
		catch(std::exception &e)
		{
			mpiErrMsg = e.what();
		}

		NRPLogger::SPDErrLogDefault("MPI Finalize failed: " + mpiErrMsg);
	}
}

MPISetup::MPISetup(int argc, char **argv)
{
	const int errc = MPI_Init(&argc, &argv);
	if(errc != 0)
		throw NRPException::logCreate("Failed to initialize MPI: " + getErrorString(errc));

	// Initialize MPI4Py
	if(import_mpi4py__MPI() != 0)
		throw NRPException::logCreate("Failed to initialize MPI4Py");

	MPISinglePropertySerializer<boost::python::object>::setPyMPICommFcn(reinterpret_cast<void*>(PyMPIComm_New));
}
