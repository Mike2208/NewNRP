#include "nrp_example_json_engine/devices/example_device.h"

const int &ExampleDevice::number() const
{	return this->getPropertyByName<"number">();	}

int &ExampleDevice::number()
{	return this->getPropertyByName<"number">();	}

const std::string &ExampleDevice::string() const
{	return this->getPropertyByName<"string">();	}

std::string &ExampleDevice::string()
{	return this->getPropertyByName<"string">();	}
