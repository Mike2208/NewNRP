#ifndef MPI_PROPERTY_SERIALIZER_METHODS_H
#define MPI_PROPERTY_SERIALIZER_METHODS_H

/*!
 *  \file mpi_property_serializer_methods.h
 *  Contains MPI Serialization methods for single types.
 *  See MPISinglePropertySerializer for details on how to implement de-/serialization methods
 */

#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

template<>
struct MPISinglePropertySerializer<char> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(char &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_CHAR, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<double> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(double &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_DOUBLE, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<float> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(float &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_FLOAT, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<int> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(int &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_INT, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<long> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(long &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_LONG, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<long double> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(long double &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_LONG_DOUBLE, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<short> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(short &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_SHORT, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<unsigned char> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(unsigned char &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_CHAR, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<unsigned short> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(unsigned short &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_SHORT, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<>
struct MPISinglePropertySerializer<unsigned long> : public MPISinglePropertySerializerGeneral
{
	static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(unsigned long &dat)
	{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_LONG, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<class PROPERTY>
requires (sizeof(PROPERTY) == sizeof(unsigned char))
struct MPISinglePropertySerializer<PROPERTY> : public MPISinglePropertySerializerGeneral
{
    static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(PROPERTY &dat)
    {	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_CHAR, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<class PROPERTY>
requires (sizeof(PROPERTY) == sizeof(int))
struct MPISinglePropertySerializer<PROPERTY> : public MPISinglePropertySerializerGeneral
{
    static inline mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(PROPERTY &dat)
    {	return mpi_prop_datatype_t<MPI_Datatype>(MPI_INT, MPIPropertyData::getMPIAddr(&dat), 1);	}
};

template<class PROPERTY>
requires (ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod<PROPERTY>() == ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_FCN)
struct MPISinglePropertySerializer<std::vector<PROPERTY> >
        : public MPISinglePropertySerializerGeneral
{
    static constexpr int getVarSizes()
    {	return 1;	}

    static void resize(MPIPropertyData &dat, std::vector<PROPERTY> &prop)
    {
        assert(dat.CurVarLIt != dat.VariableLengths.end());
        prop.resize(*(dat.CurVarLIt++));
    }

    static void saveSize(MPIPropertyData &dat, std::vector<PROPERTY> &prop)
    {	dat.VariableLengths.push_back(prop.size());	}

    template<bool SEND>
    static void serializationFcn(MPI_Comm comm, int tag, std::vector<PROPERTY> &prop)
    {
        using mpi_serializer_t = ObjectPropertySerializerMethods<MPIPropertyData>;

        MPIPropertyData dat;
        for(auto &cDat : prop)
        {
            mpi_serializer_t::template emplaceSingleObject<PROPERTY, SEND>(dat, "", mpi_serializer_t::serializeSingleProperty(cDat));

            assert(!dat.ExchangeFunctions.empty());
            dat.ExchangeFunctions.front()(comm, tag);
            dat.ExchangeFunctions.clear();
        }
    }
};

template<class PROPERTY>
requires (ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod<PROPERTY>() != ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_FCN)
struct MPISinglePropertySerializer<std::vector<PROPERTY> >
    : public MPISinglePropertySerializerGeneral
{
    static constexpr int getVarSizes()
    {	return 1;	}

    static void resize(MPIPropertyData &dat, std::vector<PROPERTY> &prop)
    {
        assert(dat.CurVarLIt != dat.VariableLengths.end());
        prop.resize(*(dat.CurVarLIt++));
    }

    static void saveSize(MPIPropertyData &dat, std::vector<PROPERTY> &prop)
    {	dat.VariableLengths.push_back(prop.size());	}

    static mpi_prop_datatype_t<MPIDerivedDatatype> derivedMPIDatatype(std::vector<PROPERTY> &prop)
    {
        constexpr auto type = ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod<PROPERTY>();
        if constexpr (type == ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_BASE)
        {
            auto elDatatype = MPISinglePropertySerializer<PROPERTY>::baseMPIDatatype(prop.front());

            MPI_Datatype datatype = MPI_DATATYPE_NULL;
            MPI_Type_contiguous(prop.size(), std::get<0>(elDatatype), &datatype);
            MPI_Type_commit(&datatype);

            return std::tuple(MPIDerivedDatatype(datatype), MPIPropertyData::getMPIAddr(prop.data()), 1);
        }
        else
        {
            static_assert (type == ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_DERIVED, "Unknown serialization type");
            auto elDatatype = MPISinglePropertySerializer<PROPERTY>::derivedMPIDatatype(prop.front());

            MPI_Datatype datatype = MPI_DATATYPE_NULL;
            MPI_Type_contiguous(prop.size(), std::get<0>(elDatatype), &datatype);
            MPI_Type_commit(&datatype);

            return std::tuple(MPIDerivedDatatype(datatype), MPIPropertyData::getMPIAddr(prop.data()), 1);
        }
    }
};

template<class PROPERTY, size_t N>
requires (ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod<PROPERTY>() == ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_FCN)
struct MPISinglePropertySerializer<std::array<PROPERTY,N> >
        : public MPISinglePropertySerializerGeneral
{
    template<bool SEND>
    static void serializationFcn(MPI_Comm comm, int tag, std::array<PROPERTY,N> &prop)
    {
        using mpi_serializer_t = ObjectPropertySerializerMethods<MPIPropertyData>;

        MPIPropertyData dat;
        for(auto &cDat : prop)
        {
            mpi_serializer_t::template emplaceSingleObject<PROPERTY, SEND>(dat, "", mpi_serializer_t::serializeSingleProperty(cDat));

            assert(!dat.ExchangeFunctions.empty());
            dat.ExchangeFunctions.front()(comm, tag);
            dat.ExchangeFunctions.clear();
        }
    }
};

template<class PROPERTY, size_t N>
requires (ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod<PROPERTY>() != ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_FCN)
struct MPISinglePropertySerializer<std::array<PROPERTY, N> >
    : public MPISinglePropertySerializerGeneral
{
    static mpi_prop_datatype_t<MPIDerivedDatatype> derivedMPIDatatype(std::array<PROPERTY,N> &prop)
    {
        constexpr auto type = ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod<PROPERTY>();
        if constexpr (type == ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_BASE)
        {
            auto elDatatype = MPISinglePropertySerializer<PROPERTY>::baseMPIDatatype(prop.front());

            MPI_Datatype datatype = MPI_DATATYPE_NULL;
            MPI_Type_contiguous(N, std::get<0>(elDatatype), &datatype);
            MPI_Type_commit(&datatype);

            return std::tuple(MPIDerivedDatatype(datatype), MPIPropertyData::getMPIAddr(prop.data()), 1);
        }
        else
        {
            static_assert (type == ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_DERIVED, "Unknown serialization type");
            auto elDatatype = MPISinglePropertySerializer<PROPERTY>::derivedMPIDatatype(prop.front());

            MPI_Datatype datatype = MPI_DATATYPE_NULL;
            MPI_Type_contiguous(N, std::get<0>(elDatatype), &datatype);
            MPI_Type_commit(&datatype);

            return std::tuple(MPIDerivedDatatype(datatype), MPIPropertyData::getMPIAddr(prop.data()), 1);
        }
    }
};


template<>
struct MPISinglePropertySerializer<std::string>
    : public MPISinglePropertySerializerGeneral
{
	static constexpr int getVarSizes();

	static void resize(MPIPropertyData &dat, std::string &prop);

	static void saveSize(MPIPropertyData &dat, std::string &prop);

	static mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(std::string &prop);
};

template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
struct MPISinglePropertySerializer<PROPERTY_TEMPLATE>
    : public MPISinglePropertySerializerGeneral
{
	static constexpr int getVarSizes()
	{	return ObjectPropertySerializerMethods<MPIPropertyData>::getNumVariableProperties<PROPERTY_TEMPLATE>();	}
};


template<>
struct MPISinglePropertySerializer<boost::python::object> : public MPISinglePropertySerializerGeneral
{
	template<bool SEND>
	static void serializationFcn(MPI_Comm comm, int tag, boost::python::object &prop)
	{
		namespace python = boost::python;

		try
		{
			auto pyComm = pyMPIComm(comm);
			python::object interComm = python::import("mpi4py.MPI").attr("Intercomm")(pyComm);

			if constexpr (SEND == true)
			{
				python::tuple args(prop);
				python::dict kwargs;
				kwargs["dest"] = 0;
				kwargs["tag"] = tag;
				interComm.attr("send")(*args, **kwargs);
			}
			else
			{
				python::tuple args;
				python::dict kwargs;
				kwargs["tag"] = tag;

				prop = interComm.attr("recv")(*args, **kwargs);
			}
		}
		catch(python::error_already_set&)
		{
			const auto errStr = "Error while serializing python object with MPI: \n" + handle_pyerror();
			std::cerr << errStr << "\n";
			throw std::runtime_error(errStr);
		}
	}

	private:
	    static boost::python::object pyMPIComm(MPI_Comm comm);
};

#endif // MPI_PROPERTY_SERIALIZER_METHODS_H
