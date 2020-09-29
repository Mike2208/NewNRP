#ifndef MPI_property_SERIALIZER_H
#define MPI_property_SERIALIZER_H

#include "nrp_general_library/utils/python_error_handler.h"
#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <assert.h>
#include <boost/python.hpp>
#include <concepts>
#include <functional>
#include <list>
#include <mpi.h>
#include <vector>

/*!
 * \brief Contains MPI data for sending/receiving
 */
struct MPIPropertyData
{
	/*!
	 * \brief Contains all information to later on use MPI_Type_struct to create a datatype for all Properties of a PropertyTemplate
	 */
	template<class PROP_DATATYPE>
	using mpi_prop_datatype_t = std::tuple<PROP_DATATYPE, MPI_Aint, int>;

	/*!
	 * \brief Function to perform more complex MPI Send/Receive tasks
	 */
	using mpi_comm_fcn_t = std::function<void(MPI_Comm,int)>;

	/*!
	 * \brief Describes how to serialize singular single property using a derived MPI Datatypes
	 * Must already be commited via MPI_Type_Commit.
	 * Note that this datatype must reference absolute address of a single Property's data
	 */
	class MPIDerivedDatatype
	{
		public:
			/*!
			 * \brief Constructor
			 * \param datatype MPI Datatype. Must already be commited via MPI_Type_Commit
			 */
			MPIDerivedDatatype(MPI_Datatype datatype = MPI_DATATYPE_NULL);

			/*!
			 * \brief Free datatype if this is a derived class
			 */
			~MPIDerivedDatatype();

			// Cannot be copied, only moved. This prevents accidental destruction
			MPIDerivedDatatype(MPIDerivedDatatype &&);
			MPIDerivedDatatype(const MPIDerivedDatatype &) = delete;
			MPIDerivedDatatype &operator=(MPIDerivedDatatype&&);
			MPIDerivedDatatype &operator=(const MPIDerivedDatatype&) = delete;

			operator MPI_Datatype() const;
			operator MPI_Datatype&();

		private:
			/*!
			 * \brief Describes how to serialize single property.
			 * Note that this datatype must reference absolute location of a single Property's data
			 */
			MPI_Datatype _datatype = MPI_DATATYPE_NULL;
	};

	/*!
	 * \brief MPI Datatype. Contains addresses under which property data can be retrieved
	 */
	MPIDerivedDatatype Datatype;

	/*!
	 * \brief Additional functions for sending/receiving
	 */
	std::list<mpi_comm_fcn_t> ExchangeFunctions;

	/*!
	 * \brief Additional size data. If a PropertyTemplate contains properties with a variable size,
	 * this vector will contain the lengths
	 */
	std::vector<int> VariableLengths;

	/*!
	 * \brief Iterator to current length in VariableLengths (Deserializeproperties will iterate over VariableLengths)
	 */
	std::vector<int>::const_iterator CurVarLIt = VariableLengths.begin();

	/*!
	 * \brief Generate the datatype from this class's PropDatatypes and PropDataAddresses
	 */
	void generateDatatype();

	/*!
	 * \brief Generate the datatype from this class's PropDatatypes and PropDataAddresses
	 */
	void generateDatatype(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes);

	void addPropDatatype(mpi_prop_datatype_t<MPI_Datatype> &&dat);
	void addPropDatatype(mpi_prop_datatype_t<MPIDerivedDatatype> &&dat);

	void addPropDatatype(mpi_comm_fcn_t &&dat);

	MPIPropertyData() = default;
	MPIPropertyData(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes);

	static MPI_Aint getMPIAddr(void *loc);

	private:
	    /*!
		 * \brief Keep ownership of derived datatypes
		 */
	    std::list<MPIDerivedDatatype> PropDerivedDatatypes;

		// Information for creating Datatype using MPI_Type_create_struct
		std::vector<MPI_Datatype> PropDatatypes;
		std::vector<int> PropCounts;
		std::vector<MPI_Aint> PropAddresses;
};

/*!
 * \brief Base for all MPI single Property serializer classes
 */
struct MPISinglePropertySerializerGeneral
{
	template<class PROP_DATATYPE>
	using mpi_prop_datatype_t = MPIPropertyData::mpi_prop_datatype_t<PROP_DATATYPE>;
	using MPIDerivedDatatype = MPIPropertyData::MPIDerivedDatatype;
	using mpi_comm_fcn_t = MPIPropertyData::mpi_comm_fcn_t;

	static inline MPI_Aint getMPIAddr(void *loc)
	{	return MPIPropertyData::getMPIAddr(loc);	}
};

/*!
 * \brief MPI Single Property Serializer. Must be defined for each property
 * \tparam PROPERTY Property for which to define de-/serialization functions
 */
template<class PROPERTY>
struct MPISinglePropertySerializer
        : public MPISinglePropertySerializerGeneral
{
/*! \fn static mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(PROPERTY &prop)
 * \brief Define if this type is based on a base MPI_Datatype.
 * \return Returns tuple conttaining (MPI_Datatype, Address of property, Count(usually 1, unless PROPERTY is an array))
 */

/*! \fn static mpi_prop_datatype_t<MPIDerivedDatatype> derivedMPIDatatype(PROPERTY &prop)
 * \brief Define if this type is based on a derived MPI_Datatype.
 * \return Returns tuple conttaining (MPIDerivedDatatype, Address of property, Count(usually 1, unless PROPERTY is an array))
 */

/*! \fn template<bool SEND> static void serializationFcn(MPI_Comm comm, int tag, PROPERTY &prop)
 * \brief If a PROPERTY type cannot be sent/received normally, a function can be defined to perform the sending/recevingin
 * \tparam SEND Should this function send (SEND=true) or receveive (SEND=false) data
 * \param comm MPI Communicator to use for sending/receiving
 * \param tag MPI Message tag to send/receive
 * \param prop Property data to send
 */

/*! \fn static void resize(MPIPropertyData &dat, PROPERTY &prop)
 * \brief Define if property must be resized before deseriaization. Usually only required for vectors.
 * \param dat Deserialization data. It contains dat.VariableLengths and dat.CurVarLIt, which can be used to find the size that prop must take
 * \param prop Property to resize
 */

/*! \fn static void saveSize(MPIPropertyData &dat, PROPERTY &prop)
 * \brief Define if property must be resized before deseriaization. Usually only required for vectors.
 * \param dat Serialization data. It contains dat.VariableLengths and dat.CurVarLIt, which can be used to store the size of prop
 * \param prop Size of property to get
 */

/*! \fn static constexpr int getVarSizes()
 * \brief Define if property can contain a variable number of entries
 * \param dat Serialization data. It contains dat.VariableLengths, which is adjusted depending on the number of variable properties
 * \return Returns number of variable entries
 */

};

template<>
class ObjectPropertySerializerMethods<MPIPropertyData>
        : public PropertySerializerGeneral
{
	public:
		enum MPI_PROP_SERIALIZATION_METHOD
		{	MPI_PROP_SERIALIZATION_BASE, MPI_PROP_SERIALIZATION_DERIVED, MPI_PROP_SERIALIZATION_FCN	};

		using MPIDerivedDatatype = MPIPropertyData::MPIDerivedDatatype;
		using mpi_comm_fcn_t = MPIPropertyData::mpi_comm_fcn_t;

		template<class PROP_DATA_TYPE>
		using mpi_prop_datatype_t = MPIPropertyData::mpi_prop_datatype_t<PROP_DATA_TYPE>;

		using ObjectDeserializer = typename PropertySerializerGeneral::ObjectDeserializer<MPIPropertyData>;

		template<class PROPERTY_TEMPLATE>
		static constexpr int getNumVariableProperties()
		{	return getNumVariableProperties<PROPERTY_TEMPLATE, 0>();	}

		/*!
		 * \brief Only passes along property. Does nothing more
		 * \tparam PROPERTY Type to serialize
		 * \param property Data to serialize
		 * \return Returns ref to property
		 */
		template<class PROPERTY>
		static constexpr PROPERTY *serializeSingleProperty(PROPERTY &property)
		{	return &property;	}

		template<class PROPERTY>
		static constexpr MPI_PROP_SERIALIZATION_METHOD serializationMethod();

		/*!
		 * \brief Add single property information to data
		 * \tparam PROP_DATA Property information type. Can be either mpi_prop_datatype_t or mpi_comm_fcn_t
		 * \tparam SEND Should object be emplaced for sending (1) or receiving (0). Only changes the serializationFcn
		 * \param data Total PropertyTemplate information. singlePropData will be inserted here
		 * \param name Name of current property
		 * \param singlePropData Information about single property
		 */
		template<class PROPERTY, bool SEND = true>
		static void emplaceSingleObject(MPIPropertyData &data, const std::string_view &, PROPERTY *property)
		{
			assert(property != nullptr);

			constexpr bool isResizable = requires ()
			{	MPISinglePropertySerializer<PROPERTY>::saveSize(data, *property);	};
			if constexpr (isResizable)
			{	MPISinglePropertySerializer<PROPERTY>::saveSize(data, *property);	}

			constexpr auto serMethod = serializationMethod<PROPERTY>();
			if constexpr (serMethod == MPI_PROP_SERIALIZATION_BASE)
			{	data.addPropDatatype(MPISinglePropertySerializer<PROPERTY>::baseMPIDatatype(*property));	}
			else if constexpr (serMethod == MPI_PROP_SERIALIZATION_DERIVED)
			{	data.addPropDatatype(MPISinglePropertySerializer<PROPERTY>::derivedMPIDatatype(*property));	}
			else
			{
				static_assert(serMethod == MPI_PROP_SERIALIZATION_FCN, "Unknown serialization method specified");
				data.addPropDatatype(std::bind(&MPISinglePropertySerializer<PROPERTY>::template serializationFcn<SEND>, std::placeholders::_1, std::placeholders::_2, *property));
			}
		}

		/*!
		 * \brief Prepare single property for data receiving. Will resize vectors and other containers to proper size and insert corresponding datatype into data
		 * \param data MPI Data to determine property resize if required
		 * \return Returns a PROPERTY type of sufficient size to receive upcoming MPI_RECV
		 */
		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(MPIPropertyData &data, const std::string_view &name)
		{
			auto retVal = resizeIfVariable<PROPERTY>(data);
			emplaceSingleObject<PROPERTY, false>(data, name, serializeSingleProperty(retVal));

			return retVal;
		}

	private:
		template<class PROPERTY>
		static inline PROPERTY resizeIfVariable(MPIPropertyData &data)
		{
			constexpr bool isResizable = requires (MPIPropertyData &dat, PROPERTY &prop)
			{	MPISinglePropertySerializer<PROPERTY>::resize(dat, prop);	};

			if constexpr (isResizable)
			{
				auto retVal = PROPERTY();
				MPISinglePropertySerializer<PROPERTY>::resize(data, retVal);
				return retVal;
			}
			else
			{	return PROPERTY();	}
		}

		template<class PROPERTY_TEMPLATE, int ID>
		static constexpr int getNumVariableProperties()
		{
			if constexpr (ID < PROPERTY_TEMPLATE::NumProperties)
			{
				using property_t = typename PROPERTY_TEMPLATE::template property_t<ID>;
				constexpr bool isVariable = requires ()
				{	MPISinglePropertySerializer<property_t>::getVarSizes();	};

				if constexpr (isVariable)
				{	return MPISinglePropertySerializer<property_t>::getVarSizes() + getNumVariableProperties<PROPERTY_TEMPLATE, ID+1>();	}
				else
				{	return getNumVariableProperties<PROPERTY_TEMPLATE, ID+1>();	}
			}
			else
			{	return 0;	}
		}
};

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


template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
using MPIPropertySerializer = PropertySerializer<MPIPropertyData, PROPERTY_TEMPLATE>;

template<class PROPERTY>
constexpr ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_METHOD ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod()
{
	constexpr bool has_serialize_fcn = requires (MPI_Comm comm, int tag, PROPERTY &prop)
	{	MPISinglePropertySerializer<PROPERTY>::template serializationFcn<true>(comm, tag, prop);	};

	constexpr bool has_base_type = requires (PROPERTY &prop)
	{	MPISinglePropertySerializer<PROPERTY>::baseMPIDatatype(prop);	};

	if constexpr (has_serialize_fcn)
	{	return MPI_PROP_SERIALIZATION_FCN;	}
	else if constexpr (has_base_type)
	{	return MPI_PROP_SERIALIZATION_BASE;	}
	else
	{
		constexpr bool has_derived_type = requires (PROPERTY &prop)
		{	MPISinglePropertySerializer<PROPERTY>::derivedMPIDatatype(prop);	};
		static_assert(has_derived_type, "No proper serialization method found for this PROPERTY type");
		return MPI_PROP_SERIALIZATION_DERIVED;
	}
}

#endif
