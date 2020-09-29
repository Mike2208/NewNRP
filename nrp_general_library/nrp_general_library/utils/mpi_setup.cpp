#include "nrp_general_library/utils/mpi_setup.h"

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

MPISetup *MPISetup::initializeOnce(int argc, char **argv)
{
	if(MPISetup::getInstance() == nullptr)
		return MPISetup::resetInstance(argc, argv);

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
