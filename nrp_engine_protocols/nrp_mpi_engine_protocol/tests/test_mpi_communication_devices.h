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

