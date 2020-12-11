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
#include "nrp_general_library/utils/nrp_exceptions.h"

void MPICommunication::sendString(MPI_Comm comm, int tag, const std::string &str)
{
	sendMPI(str.data(), str.size(), MPI_CHAR, 0, tag, comm);
}

std::string MPICommunication::recvString(MPI_Comm comm, int tag)
{
	MPI_Status status;
	MPI_Probe(MPI_ANY_SOURCE, tag, comm, &status);

	int strLen;
	MPI_Get_count(&status, MPI_CHAR, &strLen);

	std::string retVal;
	retVal.resize(strLen);

	recvMPI(retVal.data(), strLen, MPI_CHAR, MPI_ANY_SOURCE, tag, comm);

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

	dat.generateDatatype();

	sendMPI(MPI_BOTTOM, 1, dat.Datatype, 0, tag, comm);
}

DeviceIdentifier MPICommunication::recvDeviceID(MPI_Comm comm, int tag)
{
	MPIPropertyData dat;
	dat.VariableLengths.resize(3);

	recvMPI(dat.VariableLengths.data(), 3, MPI_INT, 0, tag, comm);

	DeviceIdentifier id("", "", "");
	id.Name.resize(dat.VariableLengths[0]);
	id.Type.resize(dat.VariableLengths[1]);
	id.EngineName.resize(dat.VariableLengths[2]);

	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.Name);
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.Type);
	ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject<std::string, false>(dat, "", &id.EngineName);

	dat.generateDatatype();

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
		throw NRPException::logCreate(e, "Error while sending MPI data");
	}
}

void MPICommunication::sendMPIBlocking(const void *buff, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	try
	{
		const int errc = MPI_Ssend(buff, count, datatype, dest, tag, comm);
		if(errc != MPI_SUCCESS)
			throw std::runtime_error(MPISetup::getErrorString(errc));
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Error while sending MPI data");
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
		throw NRPException::logCreate(e, "Error while receving MPI data");
	}
}
