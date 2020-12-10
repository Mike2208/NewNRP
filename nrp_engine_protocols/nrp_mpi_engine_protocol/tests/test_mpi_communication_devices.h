/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef TEST_MPI_COMMUNICATION_DEVICES_H
#define TEST_MPI_COMMUNICATION_DEVICES_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/utils/property_template.h"

#include <boost/python.hpp>
#include <vector>

class TestDevice1
        : public DeviceInterface,
          public PropertyTemplate<TestDevice1, PropNames<"int", "string", "py">, int, std::string, boost::python::object>
{
	public:
		static constexpr FixedString TypeName = "test1";

		static constexpr int DefInt = 3;
		static constexpr std::string_view DefStr = "testStr";

		TestDevice1(int intVal = DefInt, std::string strVal = std::string(DefStr), boost::python::object pyVal = boost::python::dict());

		int &getInt();
		const int &getInt() const;

		std::string &getStr();
		const std::string &getStr() const;

		boost::python::object &getPy();
		const boost::python::object &getPy() const;
};


class TestDevice2
        : public DeviceInterface,
          public PropertyTemplate<TestDevice2, PropNames<"int", "vec", "py">, int, std::vector<int>, boost::python::object>
{
	public:
		static constexpr FixedString TypeName = "test2";

		TestDevice2() = default;

		int &getInt();
		const int &getInt() const;

		std::vector<int> &getVec();
		const std::vector<int> &getVec() const;

		boost::python::object &getPy();
		const boost::python::object &getPy() const;
};

#endif // TEST_MPI_COMMUNICATION_DEVICES_H

