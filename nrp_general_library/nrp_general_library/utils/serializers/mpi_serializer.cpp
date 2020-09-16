#include "nrp_general_library/utils/serializers/mpi_serializer.h"

#include "mpi4py/mpi4py.MPI_api.h"

#include <assert.h>
#include <boost/python.hpp>
#include <Python.h>

ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::MPIDerivedDatatype(MPI_Datatype datatype)
    : _datatype(datatype)
{}


ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::~MPIDerivedDatatype()
{
	MPI_Type_free(&this->_datatype);
}

ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::MPIDerivedDatatype(MPIDerivedDatatype &&other)
    : _datatype(other._datatype)
{
	other._datatype = MPI_DATATYPE_NULL;
}

ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype &ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::operator=(MPIDerivedDatatype &&other)
{
	this->~MPIDerivedDatatype();

	this->_datatype = other._datatype;
	other._datatype = MPI_DATATYPE_NULL;

	return *this;
}

ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::operator MPI_Datatype() const
{	return this->_datatype;	}

ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype::operator MPI_Datatype&()
{	return this->_datatype;	}

MPI_Aint ObjectPropertySerializerMethods<MPIPropertyData>::getMPIAddr(const void *loc)
{
	MPI_Aint mpiLoc;
	MPI_Get_address(loc, &mpiLoc);

	return mpiLoc;
}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<true, boost::python::object>(boost::python::object &data)
{
	namespace python = boost::python;
	return [data](MPI_Comm comm)
	{
		python::object mpi4py = python::import("mpi4py.MPI");

		assert(PyMPIComm_New != nullptr);
		python::object pyComm(python::handle<>(python::borrowed(PyMPIComm_New(comm))));
		python::object intercomm = mpi4py.attr("Intercomm")(pyComm);

		intercomm.attr("");
	};
}



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
	this->Datatype = mpi_data_t(datatype);
}

void MPIPropertyData::addPropDatatype(MPIPropertyData::mpi_comm_fcn_t &&dat)
{
	this->ExchangeFunctions.push_back(std::move(dat));
}

void MPIPropertyData::addPropDatatype(MPIPropertyData::mpi_data_t &&type, MPI_Aint address, int count)
{
	this->PropDerivedDatatypes.push_back(std::move(type));

	this->addPropDatatype((MPI_Datatype)this->PropDerivedDatatypes.back(), address, count);
}

void MPIPropertyData::addPropDatatype(MPI_Datatype type, MPI_Aint address, int count)
{
	this->PropDatatypes.push_back(type);
	this->PropAddresses.push_back(address);
	this->PropCounts.push_back(count);
}

MPIPropertyData::MPIPropertyData(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes)
{
	this->generateDatatype(count, dataCounts, dataAddresses, datatypes);
}
