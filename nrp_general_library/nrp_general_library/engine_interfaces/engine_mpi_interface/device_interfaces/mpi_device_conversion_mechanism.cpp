#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"

#include "nrp_general_library/utils/mpi_setup.h"

MPIDeviceData::MPIDeviceData(DeviceIdentifier _deviceID, MPIPropertyData &&_dat)
    : MPIPropertyData(std::move(_dat)),
      DeviceID(_deviceID)
{}

void MPICommunication::sendString(MPI_Comm comm, int tag, const std::string &str)
{
	sendMPI(str.data(), str.size()+1, MPI_CHAR, 0, tag, comm);
}

std::string MPICommunication::recvString(MPI_Comm comm, int tag)
{
	MPI_Status status;
	MPI_Probe(MPI_ANY_SOURCE, tag, comm, &status);

	int strLen;
	MPI_Get_count(&status, MPI_CHAR, &strLen);

	std::string retVal;
	retVal.resize(strLen);

	recvMPI(&retVal, strLen, MPI_CHAR, MPI_ANY_SOURCE, tag, comm);

	return retVal;
}

void MPICommunication::sendDeviceID(MPI_Comm comm, int tag, const DeviceIdentifier &id)
{
	MPIPropertyData dat;
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string>(dat, "", const_cast<std::string*>(&id.Name));
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string>(dat, "", const_cast<std::string*>(&id.Type));
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string>(dat, "", const_cast<std::string*>(&id.EngineName));

	assert(dat.VariableLengths.size() == 3);
	sendMPI(dat.VariableLengths.data(), 3, MPI_INT, 0, tag, comm);

	sendMPI(MPI_BOTTOM, 1, dat.Datatype, 0, tag, comm);
}

DeviceIdentifier MPICommunication::recvDeviceID(MPI_Comm comm, int tag)
{
	MPIPropertyData dat;
	dat.VariableLengths.resize(3);

	recvMPI(dat.VariableLengths.data(), 3, MPI_INT, 0, tag, comm);

	DeviceIdentifier id("", "", "");
	id.Name.resize(dat.VariableLengths[0]);
	id.Type.resize(dat.VariableLengths[0]);
	id.EngineName.resize(dat.VariableLengths[0]);

	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.Name);
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.Type);
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.EngineName);

	recvMPI(MPI_BOTTOM, 1, dat.Datatype, 0, tag, comm);

	return id;
}

void MPICommunication::sendMPI(const void *buff, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	try
	{
		const int errc = MPI_Send(buff, count, datatype, dest, tag, comm);
		if(errc != MPI_SUCCESS)
			throw std::runtime_error(MPISetup::getErrorString(errc));
	}
	catch(std::exception &e)
	{
		const auto errMsg = std::string("Error while sending MPI data: \n") + e.what();
		std::cerr << errMsg << std::endl;
		throw std::runtime_error(errMsg);
	}
}

void MPICommunication::recvMPI(void *buff, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm)
{
	try
	{
		const int errc = MPI_Recv(buff, count, datatype, source, tag, comm, nullptr);
		if(errc != MPI_SUCCESS)
			throw std::runtime_error(MPISetup::getErrorString(errc));
	}
	catch(std::exception &e)
	{
		const auto errMsg = std::string("Error while receving MPI data: \n") + e.what();
		std::cerr << errMsg << std::endl;
		throw std::runtime_error(errMsg);
	}
}
