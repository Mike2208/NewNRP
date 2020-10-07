#include "nrp_general_library/utils/serializers/mpi_property_serializer_methods.h"

#include <boost/python.hpp>
#include <mpi.h>
#include <mpi4py/mpi4py.MPI_api.h>
#include <Python.h>

// Silence warning regarding unused fcn. (import_mpi4py__MPI is called via MPISetup)
constexpr auto imp_fcn = import_mpi4py__MPI;

void MPISinglePropertySerializer<std::string>::resize(MPIPropertyData &dat, std::string &prop)
{
	assert(dat.CurVarLIt != dat.VariableLengths.end());
	prop.resize(*(dat.CurVarLIt++));
}

void MPISinglePropertySerializer<std::string>::saveSize(MPIPropertyData &dat, std::string &prop)
{	dat.VariableLengths.push_back(prop.size()+1);	}

auto MPISinglePropertySerializer<std::string>::baseMPIDatatype(std::string &prop) -> mpi_prop_datatype_t<MPI_Datatype>
{
	auto retVal = MPISinglePropertySerializer<char>::baseMPIDatatype(*prop.data());
	std::get<2>(retVal) = prop.capacity();
	return retVal;
}

boost::python::object MPISinglePropertySerializer<boost::python::object>::pyMPIComm(MPI_Comm comm)
{
	assert(PyMPIComm_New != nullptr);

	namespace python = boost::python;
	return python::object(python::handle<>(python::borrowed((*PyMPIComm_New)(comm))));
}

constexpr int MPISinglePropertySerializer<std::string>::getVarSizes()
{	return 1;	}
