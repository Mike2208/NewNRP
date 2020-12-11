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

#include <boost/python.hpp>

#include "nrp_general_library/config/cmake_constants.h"
#include "nrp_general_library/device_interface/python_device.h"
#include "nrp_nest_server_engine/config/cmake_constants.h"
#include "nrp_nest_server_engine/devices/nest_server_device.h"
#include "nrp_nest_server_engine/python/create_device_class.h"


namespace python = boost::python;

static CreateDeviceClass *pCreateDevice = nullptr;

python::object CreateDevice(python::tuple args, python::dict kwargs)
{
	assert(pCreateDevice != nullptr);
	return pCreateDevice->createAndRegisterDevice(args, kwargs);
}

void RegisterDevice(python::str devName, python::object nodeCollection)
{
	assert(pCreateDevice != nullptr);
	return pCreateDevice->registerDevice(devName, nodeCollection);
}

python::dict GetDevMap()
{
	assert(pCreateDevice != nullptr);
	return pCreateDevice->pyDevMap();
}

void setNestData(NestServerDevice &dev, const boost::python::object &data)
{	dev.data() = data;	}

const boost::python::object &getNestData(const NestServerDevice &dev)
{	return dev.data();	}

BOOST_PYTHON_MODULE(NRP_NEST_PYTHON_MODULE)
{
	// Import General NRP Python Module
	python::import(PYTHON_MODULE_NAME_STR);

	python_property_device_class<NestServerDevice>::create()
	        .add_property("data", python::make_function(getNestData, python::return_value_policy<python::copy_const_reference>()), &setNestData);

	// Setup CreateDevice and import Nest
	python::class_<CreateDeviceClass>("__CreateDeviceClass", python::no_init)
	        .def("CreateDevice", python::raw_function(&CreateDeviceClass::pyCreateDevice))
	        .def("RegisterDevice", python::raw_function(&CreateDeviceClass::pyRegisterDevice))
	        .def("GetDevMap", &CreateDeviceClass::pyDevMap);

	python::dict devMap;
	python::dict nestDict(python::import("nest").attr("__dict__"));

	python::object pyCreateDevice(CreateDeviceClass(nestDict, devMap));
	python::scope().attr("__CreateDevice") = pyCreateDevice;

	CreateDeviceClass &createDevice = python::extract<CreateDeviceClass&>(pyCreateDevice);
	pCreateDevice = &createDevice;

	// Setup Nest Create and Register Device Function
	python::def("CreateDevice", python::raw_function(CreateDevice));
	python::def("RegisterDevice", RegisterDevice);
	python::def("GetDevMap", GetDevMap);
}
