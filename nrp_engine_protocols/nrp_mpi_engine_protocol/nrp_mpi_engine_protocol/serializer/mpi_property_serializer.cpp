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

#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/mpi_setup.h"

#include <assert.h>
#include <mpi.h>

MPIPropertyData::MPIDerivedDatatype::MPIDerivedDatatype(MPI_Datatype datatype)
    : _datatype(datatype)
{}


MPIPropertyData::MPIDerivedDatatype::~MPIDerivedDatatype()
{
	if(this->_datatype != MPI_DATATYPE_NULL)
		MPI_Type_free(&this->_datatype);
}

MPIPropertyData::MPIDerivedDatatype::MPIDerivedDatatype(MPIDerivedDatatype &&other)
    : _datatype(other._datatype)
{
	other._datatype = MPI_DATATYPE_NULL;
}

MPIPropertyData::MPIDerivedDatatype &ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::operator=(MPIDerivedDatatype &&other)
{
	this->~MPIDerivedDatatype();

	this->_datatype = other._datatype;
	other._datatype = MPI_DATATYPE_NULL;

	return *this;
}

MPIPropertyData::MPIDerivedDatatype::operator MPI_Datatype() const
{	return this->_datatype;	}

MPIPropertyData::MPIDerivedDatatype::operator MPI_Datatype&()
{	return this->_datatype;	}

void MPIPropertyData::generateDatatype()
{
	assert(this->PropDatatypes.size() == this->PropAddresses.size());
	assert(this->PropDatatypes.size() == this->PropCounts.size());

	this->generateDatatype(this->PropDatatypes.size(), this->PropCounts.data(), this->PropAddresses.data(), this->PropDatatypes.data());
}

void MPIPropertyData::generateDatatype(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes)
{
	MPI_Datatype datatype;
	MPI_Type_create_struct(count, dataCounts, dataAddresses, datatypes, &datatype);

	MPI_Type_commit(&datatype);
	this->Datatype = MPIDerivedDatatype(datatype);

	assert(this->Datatype != MPI_DATATYPE_NULL);
}

void MPIPropertyData::addPropDatatype(mpi_prop_datatype_t<MPI_Datatype> &&dat)
{
	this->PropDatatypes.push_back(std::get<0>(dat));
	this->PropAddresses.push_back(std::get<1>(dat));
	this->PropCounts.push_back(std::get<2>(dat));
}

void MPIPropertyData::addPropDatatype(mpi_prop_datatype_t<MPIPropertyData::MPIDerivedDatatype> &&dat)
{
	this->PropDerivedDatatypes.push_back(std::move(std::get<0>(dat)));

	this->PropDatatypes.push_back(this->PropDerivedDatatypes.back());
	this->PropAddresses.push_back(std::get<1>(dat));
	this->PropCounts.push_back(std::get<2>(dat));
}

void MPIPropertyData::addPropDatatype(MPIPropertyData::mpi_comm_fcn_t &&dat)
{
	this->ExchangeFunctions.push_back(std::move(dat));
}

MPIPropertyData::MPIPropertyData(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes)
{
	this->generateDatatype(count, dataCounts, dataAddresses, datatypes);
}

MPI_Aint MPIPropertyData::getMPIAddr(void *loc)
{
	MPI_Aint addr;
	int errc = MPI_Get_address(loc, &addr);
	if(errc != 0)
		throw NRPException::logCreate("Failed to get MPI Address: " + MPISetup::getErrorString(errc));

	return addr;
}

MPIDeviceData::MPIDeviceData(DeviceIdentifier _deviceID, MPIPropertyData &&_dat)
    : MPIPropertyData(std::move(_dat)),
      DeviceID(_deviceID)
{}
