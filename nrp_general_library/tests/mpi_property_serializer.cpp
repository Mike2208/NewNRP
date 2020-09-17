#include <gtest/gtest.h>

#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

class TestProperty
        : public PropertyTemplate<TestProperty, PropNames<"int", "string">, int, std::string>
{
	public:
		TestProperty()
		    : PropertyTemplate(0, "")
		{}

		TestProperty(const PropertyTemplate &props)
		    : PropertyTemplate(props)
		{}

		TestProperty(int intDat, std::string strDat)
		    : PropertyTemplate(intDat, strDat)
		{}

		int &intDat()
		{	return PropertyTemplate::getPropertyByName<"int">();	}

		const int &intDat() const
		{	return PropertyTemplate::getPropertyByName<"int">();	}

		std::string &strDat()
		{	return PropertyTemplate::getPropertyByName<"string">();	}

		const std::string &strDat() const
		{	return PropertyTemplate::getPropertyByName<"string">();	}
};


TEST(MPI_Serializer, TestSerialization)
{
	using mpi_serializer_t = MPIPropertySerializer<TestProperty>;
	TestProperty prop(1, "2");

	MPIPropertyData data = mpi_serializer_t::serializeProperties(prop);
}
