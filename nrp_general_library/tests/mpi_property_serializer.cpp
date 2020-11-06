#include <gtest/gtest.h>

#include "nrp_general_library/utils/mpi_setup.h"
#include "nrp_general_library/utils/python_interpreter_state.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

class TestProperty
        : public PropertyTemplate<TestProperty, PropNames<"int", "string", "vec", "python">, int, std::string, std::vector<int>, boost::python::object >
{
	public:
		TestProperty()
		    : PropertyTemplate(0, "", std::vector<int>({}), boost::python::object())
		{}

		TestProperty(const PropertyTemplate &props)
		    : PropertyTemplate(props)
		{}

		TestProperty(int intDat, std::string strDat, std::vector<int> vecDat, boost::python::object pyDat)
		    : PropertyTemplate(intDat, strDat, vecDat, pyDat)
		{}

		int &intDat()
		{	return PropertyTemplate::getPropertyByName<"int">();	}

		const int &intDat() const
		{	return PropertyTemplate::getPropertyByName<"int">();	}

		std::string &strDat()
		{	return PropertyTemplate::getPropertyByName<"string">();	}

		const std::string &strDat() const
		{	return PropertyTemplate::getPropertyByName<"string">();	}

		std::vector<int> &vecDat()
		{	return PropertyTemplate::getPropertyByName<"vec">();	}

		const std::vector<int> &vecDat() const
		{	return PropertyTemplate::getPropertyByName<"vec">();	}
};


TEST(MPI_Serializer, TestSerialization)
{
	const char *pMPIArgv = "TestProg";
	PythonInterpreterState(1, const_cast<char**>(&pMPIArgv));
	MPISetup::initializeOnce(1, const_cast<char**>(&pMPIArgv));

	using mpi_serializer_t = MPIPropertySerializer<TestProperty>;
	TestProperty prop(1, "2", {3}, boost::python::dict());

	MPIPropertyData data = mpi_serializer_t::serializeProperties(prop);

	data.generateDatatype();
	ASSERT_NE(data.Datatype, MPI_DATATYPE_NULL);
	ASSERT_EQ(data.VariableLengths.size(), 2);
	ASSERT_EQ(data.VariableLengths.at(0), prop.strDat().size());
	ASSERT_EQ(data.VariableLengths.at(1), prop.vecDat().size());
	ASSERT_EQ(data.ExchangeFunctions.size(), 1);
}

TEST(MPI_Serializer, TestDeserialization)
{
	const char *pMPIArgv = "TestProg";
	PythonInterpreterState(1, const_cast<char**>(&pMPIArgv));
	MPISetup::initializeOnce(1, const_cast<char**>(&pMPIArgv));

	using mpi_serializer_t = MPIPropertySerializer<TestProperty>;
	TestProperty prop(1, "2", {3}, boost::python::dict());

	MPIPropertyData data = mpi_serializer_t::serializeProperties(prop);
	data.ExchangeFunctions.clear();

	data.CurVarLIt = data.VariableLengths.begin();
	mpi_serializer_t::updateProperties(prop, data);

	data.generateDatatype();
	ASSERT_NE(data.Datatype, MPI_DATATYPE_NULL);
	ASSERT_EQ(data.ExchangeFunctions.size(), 1);
}
