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

#include "tests/test_mpi_communication_devices.h"

TestDevice1::TestDevice1(int intVal, std::string strVal, boost::python::object pyVal)
    : DeviceInterface("", "", TypeName),
      PropertyTemplate(intVal, strVal, pyVal)
{}

int &TestDevice1::getInt()
{	return this->getPropertyByName<"int">();	}

const int &TestDevice1::getInt() const
{	return this->getPropertyByName<"int">();	}

std::string &TestDevice1::getStr()
{	return this->getPropertyByName<"string">();	}

const std::string &TestDevice1::getStr() const
{	return this->getPropertyByName<"string">();	}

boost::python::object &TestDevice1::getPy()
{	return this->getPropertyByName<"py">();	}

const boost::python::object &TestDevice1::getPy() const
{	return this->getPropertyByName<"py">();	}

int &TestDevice2::getInt()
{	return this->getPropertyByName<"int">();	}

const int &TestDevice2::getInt() const
{	return this->getPropertyByName<"int">();	}

std::vector<int> &TestDevice2::getVec()
{	return this->getPropertyByName<"vec">();	}

const std::vector<int> &TestDevice2::getVec() const
{	return this->getPropertyByName<"vec">();	}

boost::python::object &TestDevice2::getPy()
{	return this->getPropertyByName<"py">();	}

const boost::python::object &TestDevice2::getPy() const
{	return this->getPropertyByName<"py">();	}
