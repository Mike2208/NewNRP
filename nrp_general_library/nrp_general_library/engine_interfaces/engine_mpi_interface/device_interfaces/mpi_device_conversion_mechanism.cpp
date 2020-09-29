#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"

MPIDeviceData::MPIDeviceData(DeviceIdentifier _deviceID, MPIPropertyData &&_dat)
    : MPIPropertyData(std::move(_dat)),
      DeviceID(_deviceID)
{}

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
