#include "tests/test_mpi_communication_devices.h"

TestDevice1::TestDevice1(int intVal, std::string strVal, boost::python::object pyVal)
    : DeviceInterface("", TypeName, ""),
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
