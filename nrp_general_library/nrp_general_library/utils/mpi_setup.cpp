#include "nrp_general_library/utils/mpi_setup.h"

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"

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

MPISetup *MPISetup::initializeOnce(int argc, char **argv, bool sendParentPID)
{
	if(MPISetup::getInstance() == nullptr)
	{
		MPISetup *const retVal = MPISetup::resetInstance(argc, argv);

		if(sendParentPID)
		{
			MPI_Comm parentComm;
			const int errc = MPI_Comm_get_parent(&parentComm);
			if(errc != MPI_SUCCESS)
			{
				const auto errMsg = "Failed to get parent communicator: " + std::to_string(errc);
				std::cerr << errMsg << "\n";
				throw std::runtime_error(errMsg);
			}

			if(parentComm == MPI_COMM_NULL)
			{
				const auto errMsg = "No parent PID intercommunicator found. Has this process been launched via MPI_Comm_spawn()?";
				std::cerr << errMsg << "\n";
				throw std::runtime_error(errMsg);
			}

			const auto ppid = getppid();
			MPICommunication::sendMPI(&ppid, sizeof(ppid), MPI_BYTE, 0, 0, parentComm);
		}

		return retVal;
	}

	return MPISetup::getInstance();
}

std::string MPISetup::getErrorString(int MPIErrorCode)
{
	int resLen = 1024;

	std::string retVal;
	retVal.resize(resLen);

	if(MPI_Error_string(MPIErrorCode, retVal.data(), &resLen) != 0)
	{
		const auto errMsg = "Failed to read MPI Error Code " + std::to_string(MPIErrorCode);
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	return retVal;
}

MPI_Comm MPISetup::getParentComm()
{
	MPI_Comm retVal;

	const auto errc = MPI_Comm_get_parent(&retVal);
	if(errc != MPI_SUCCESS)
	{
		const auto errMsg = "Failed to get parent proces MPI Comm: " + MPISetup::getErrorString(errc);
		std::cerr << errMsg << "\n";

		retVal = MPI_COMM_NULL;
	}

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

		const auto errMsg = "MPI Finalize failed: " + mpiErrMsg;
		std::cerr << errMsg << "\n";
	}
}

MPISetup::MPISetup(int argc, char **argv)
{
	const int errc = MPI_Init(&argc, &argv);
	if(errc != 0)
	{
		const auto errMsg = "Failed to initialize MPI: " + getErrorString(errc);
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	// Initialize MPI4Py
	if(import_mpi4py__MPI() != 0)
	{
		const auto errMsg = "Failed to initialize MPI4Py";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}
}
