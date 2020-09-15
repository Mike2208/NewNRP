#include "nrp_general_library/utils/serializers/mpi_serializer.h"

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

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<char>(const char &)
{	return MPI_CHAR;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<double>(const double &)
{	return MPI_DOUBLE;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<float>(const float &)
{	return MPI_FLOAT;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<int>(const int &)
{	return MPI_INT;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<long>(const long &)
{	return MPI_LONG;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<long double>(const long double &)
{	return MPI_LONG_DOUBLE;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<short>(const short &)
{	return MPI_SHORT;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<unsigned char>(const unsigned char &)
{	return MPI_UNSIGNED_CHAR;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<unsigned short>(const unsigned short &)
{	return MPI_UNSIGNED_SHORT;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<unsigned long>(const unsigned long &)
{	return MPI_UNSIGNED_LONG;	}

template<>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType<boost::python::object>(const boost::python::object &data)
{
	namespace python = boost::python;
	return [data](MPI_Comm)
	{
		PyStruct
		if()
		 python::import("mpi4py").attr("MPI").attr("Comm").attr("Get_parent")();
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

void MPIPropertyData::addPropDatatype(MPIPropertyData::mpi_data_t type, MPI_Aint address, int count)
{
	this->PropDerivedDatatypes.push_back(type);

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
