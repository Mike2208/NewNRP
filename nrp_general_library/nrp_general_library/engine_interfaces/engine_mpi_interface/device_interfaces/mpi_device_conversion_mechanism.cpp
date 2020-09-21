#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"


void MPICommunication::sendMPIPropertyData(MPI_Comm comm, MPIPropertyData &dat)
{
	if(dat.VariableLengths.size() > 0)
	{	sendMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_UNSIGNED, 0, MPI_ANY_TAG, comm);	}

	sendMPI(MPI_BOTTOM, 1, dat.Datatype, 0, MPI_ANY_TAG, comm);

	for(const auto &exchangeFcn : dat.ExchangeFunctions)
	{	exchangeFcn(comm);	}
}

void MPICommunication::recvMPIPropertyData(MPI_Comm comm, MPIPropertyData &dat)
{
	if(dat.VariableLengths.size() > 0)
	{	recvMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, comm);	}

	recvMPI(MPI_BOTTOM, 1, dat.Datatype, MPI_ANY_SOURCE, MPI_ANY_TAG, comm);

	for(const auto &exchangeFcn : dat.ExchangeFunctions)
	{	exchangeFcn(comm);	}
}

void MPICommunication::sendDeviceID(MPI_Comm comm, const DeviceIdentifier &id)
{
	MPIPropertyData dat;
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string>(dat, "", const_cast<std::string*>(&id.Name));
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string>(dat, "", const_cast<std::string*>(&id.Type));
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string>(dat, "", const_cast<std::string*>(&id.EngineName));

	sendMPIPropertyData(comm, dat);
}

DeviceIdentifier MPICommunication::recvDeviceID(MPI_Comm comm)
{
	DeviceIdentifier id("", "", "");

	MPIPropertyData dat;
	dat.VariableLengths.resize(3);

	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.Name);
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.Type);
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.EngineName);

	recvMPIPropertyData(comm, dat);

	return id;
}
