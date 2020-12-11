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

#include "nrp_general_library/utils/serializers/mpi_property_serializer_methods.h"

#include <boost/python.hpp>
#include <mpi.h>
#include <mpi4py/mpi4py.MPI_api.h>

void *MPISinglePropertySerializer<boost::python::object>::pPyMPICommFcn = nullptr;

void MPISinglePropertySerializer<std::string>::resize(MPIPropertyData &dat, std::string &prop)
{
	assert(dat.CurVarLIt != dat.VariableLengths.end());
	prop.resize(*(dat.CurVarLIt++));
}

void MPISinglePropertySerializer<std::string>::saveSize(MPIPropertyData &dat, std::string &prop)
{	dat.VariableLengths.push_back(prop.size());	}

auto MPISinglePropertySerializer<std::string>::baseMPIDatatype(std::string &prop) -> mpi_prop_datatype_t<MPI_Datatype>
{
	auto retVal = MPISinglePropertySerializer<char>::baseMPIDatatype(*prop.data());
	std::get<2>(retVal) = prop.size();
	return retVal;
}

boost::python::object MPISinglePropertySerializer<boost::python::object>::pyMPIComm(MPI_Comm comm)
{
	assert(pPyMPICommFcn != nullptr);

	namespace python = boost::python;
	return python::object(python::handle<>(python::borrowed((*reinterpret_cast<decltype(PyMPIComm_New)>(pPyMPICommFcn))(comm))));
}

void MPISinglePropertySerializer<boost::python::object>::setPyMPICommFcn(void *fcn)
{
	MPISinglePropertySerializer::pPyMPICommFcn = fcn;
}
