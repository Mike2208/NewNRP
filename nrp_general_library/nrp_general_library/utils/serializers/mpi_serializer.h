#ifndef MPI_SERIALIZER_H
#define MPI_SERIALIZER_H

#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <assert.h>
#include <functional>
#include <list>
#include <mpi.h>
#include <vector>

struct MPIPropertyData;

template<>
class ObjectPropertySerializerMethods<MPIPropertyData>
        : public PropertySerializerGeneral
{
	public:
		/*!
		 * \brief Contains all information to later on use MPI_Type_struct to create a datatype for all Properties of a PropertyTemplate
		 */
		template<class PROP_DATATYPE>
		using mpi_prop_datatype_t = std::tuple<PROP_DATATYPE, MPI_Aint, int>;

		/*!
		 * \brief Function to perform more complex MPI Send/Receive tasks
		 */
		using mpi_comm_fcn_t = std::function<void(MPI_Comm)>;

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

		using ObjectDeserializer = typename PropertySerializerGeneral::ObjectDeserializer<MPIPropertyData>;

		/*!
		 * \brief Get information on how to serialize a single property
		 * \tparam PROPERTY Type to serialize
		 * \param property Data
		 * \return Returns a tuple. First element is the MPI_Datatype corresponding to the given property,
		 * second element is the absolute address, third element is the count (number of elements, usually 1, unless PROPERTY is a vector/array)
		 */
		template<class PROPERTY>
		auto serializeSingleProperty(const PROPERTY &property)
		{
			static_assert(std::is_invocable_v<decltype(getMPIDataType<true, PROPERTY>)>, "No MPI Datatype found for this type");
			return getMPIDataType<true, PROPERTY>(property);
		}

		/*!
		 * \brief Add single property information to data
		 * \tparam PROP_DATA Property information type. Can be either mpi_prop_datatype_t or mpi_comm_fcn_t
		 * \param data Total PropertyTemplate information. singlePropData will be inserted here
		 * \param name Name of current property
		 * \param singlePropData Information about single property
		 */
		template<class PROP_DATA>
		static void emplaceSingleObject(MPIPropertyData &data, const std::string_view &name, PROP_DATA &&singlePropData);

		/*!
		 * \brief Prepare single property for data receiving. Will resize vectors and other containers to proper size and insert corresponding datatype into data
		 * \param data MPI Data to determine property resize if required
		 * \return Returns a PROPERTY type of sufficient size to receive upcoming MPI_RECV
		 */
		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(MPIPropertyData &data, const std::string_view &name)
		{
			auto retVal = resizeIfVariableSize(PROPERTY(), data);

			emplaceSingleObject(data, name, getMPIDataType<false>(retVal));

			return retVal;
		}

	private:

		/*!
		 * \brief All datatypes that have already been stored
		 */
		std::map<std::string_view, MPIDerivedDatatype> _propDataTypes;

		template<class PROPERTY>
		static PROPERTY resizeIfVariableSize(PROPERTY &&prop, const MPIPropertyData &)
		{	return prop;	}

		template<class PROPERTY>
		static PROPERTY resizeIfVariableSize(std::vector<PROPERTY> &&prop, MPIPropertyData &dat);

		/*!
		 * \brief Get address of loc in a format usable by MPI
		 * \param loc Memory location
		 * \return Returns address of loc in MPI_Aint format
		 */
		static MPI_Aint getMPIAddr(const void *loc);

		/*!
		 * \brief Get MPI_Datatype associated with a PROPERTY type
		 * \tparam SEND Is datatype used for send or receive? Mainly used for function creation
		 * \tparam PROPERTY Property type. MPI_Datatype will reflect this
		 * \return Returns tuple of either MPIDerivedDatatype
		 */
		template<bool SEND, class PROPERTY>
		static auto getMPIDataType(PROPERTY &);

		template<bool SEND>
		auto getMPIDataType(char &dat)
		{	return std::tuple(MPI_CHAR, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(double &dat)
		{	return std::tuple(MPI_DOUBLE, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(float &dat)
		{	return std::tuple(MPI_FLOAT, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(int &dat)
		{	return std::tuple(MPI_INT, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(long &dat)
		{	return std::tuple(MPI_LONG, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(long double &dat)
		{	return std::tuple(MPI_LONG_DOUBLE, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(short &dat)
		{	return std::tuple(MPI_SHORT, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(unsigned char &dat)
		{	return std::tuple(MPI_UNSIGNED_CHAR, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(unsigned short &dat)
		{	return std::tuple(MPI_UNSIGNED_SHORT, getMPIAddr(&dat), 1);	}

		template<bool SEND>
		auto getMPIDataType(unsigned long &dat)
		{	return std::tuple(MPI_UNSIGNED_LONG, getMPIAddr(&dat), 1);	}

		template<bool SEND, class PROPERTY>
		static auto getMPIDataType(std::vector<PROPERTY> &vec);

		template<bool SEND, class PROPERTY, auto N>
		static auto getMPIDataType(std::array<PROPERTY, N> &vec);

		/*!
		 * \brief Handles vector send/receive if elements requires special MPI functions
		 * \tparam SEND Is this a Send or Receive operation?
		 * \tparam VECTOR Vector type
		 * \param comm MPI Communication
		 * \param vec Vector to handle
		 */
		template<bool SEND, class VECTOR>
		static void handleVectorFcn(MPI_Comm comm, VECTOR &vec);
};

/*!
 * \brief Contains MPI data for sending/receiving
 */
struct MPIPropertyData
{
	using mpi_data_t = ObjectPropertySerializerMethods<MPIPropertyData>::MPIDerivedDatatype;
	using mpi_comm_fcn_t = ObjectPropertySerializerMethods<MPIPropertyData>::mpi_comm_fcn_t;

	template<class PROP_DATA_TYPE>
	using mpi_prop_datatype_t = ObjectPropertySerializerMethods<MPIPropertyData>::mpi_prop_datatype_t<PROP_DATA_TYPE>;

	/*!
	 * \brief MPI Datatype. Contains addresses under which property data can be retrieved
	 */
	mpi_data_t Datatype = nullptr;

	/*!
	 * \brief Additional functions for sending/receiving
	 */
	std::list<mpi_comm_fcn_t> ExchangeFunctions;

	/*!
	 * \brief Additional size data. If a PropertyTemplate contains properties with a variable size,
	 * this vector will contain the lengths
	 */
	std::vector<unsigned int> VariableLengths;

	/*!
	 * \brief Iterator to current length in VariableLengths (Deserializeproperties will iterate over VariableLengths)
	 */
	std::vector<unsigned int>::const_iterator CurVarLIt = VariableLengths.begin();

	/*!
	 * \brief Generate the datatype from this class's PropDatatypes and PropDataAddresses
	 */
	void generateDatatype();

	/*!
	 * \brief Generate the datatype from this class's PropDatatypes and PropDataAddresses
	 */
	void generateDatatype(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes);

	template<class PROP_DATA>
	void addPropDatatype(mpi_prop_datatype_t<PROP_DATA> dat)
	{	this->addPropDatatype(std::get<0>(dat), std::get<1>(dat), std::get<2>(dat));	}

	void addPropDatatype(mpi_comm_fcn_t &&dat);

	/*!
	 * \brief Add property data to relevant vectors
	 * \param type Property Type
	 * \param address Property Address (absolute)
	 * \param count Number of elements stored, usually 1
	 */
	void addPropDatatype(mpi_data_t &&type, MPI_Aint address, int count);

	/*!
	 * \brief Add property data to relevant vectors
	 * \param type Property Type
	 * \param address Property Address (absolute)
	 * \param count Number of elements stored, usually 1
	 */
	void addPropDatatype(MPI_Datatype type, MPI_Aint address, int count);

	MPIPropertyData();
	MPIPropertyData(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes);

	private:
	    /*!
		 * \brief Keep ownership of derived datatypes
		 */
	    std::list<mpi_data_t> PropDerivedDatatypes;

		// Information for creating Datatype using MPI_Type_create_struct
	    std::vector<MPI_Datatype> PropDatatypes;
		std::vector<int> PropCounts;
		std::vector<MPI_Aint> PropAddresses;
};

template<class PROP_DATA>
void ObjectPropertySerializerMethods<MPIPropertyData>::emplaceSingleObject(MPIPropertyData &data, const std::string_view &, PROP_DATA &&singlePropData)
{	data.addPropDatatype(std::forward<PROP_DATA>(singlePropData));	}

template<class PROPERTY>
PROPERTY ObjectPropertySerializerMethods<MPIPropertyData>::resizeIfVariableSize(std::vector<PROPERTY> &&prop, MPIPropertyData &dat)
{
	assert(dat.CurVarLIt != dat.VariableLengths.end());

	// Resize vector and increment size iterator
	prop.resize(*(dat.CurVarLIt++));

	return std::move(prop);
}

template<bool SEND, class PROPERTY>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType(std::vector<PROPERTY> &vec)
{
	// Check if vector elements must be initialized via function
	static constexpr auto (*pSubFcnType)(PROPERTY&) = getMPIDataType<SEND>;
	if constexpr (std::is_same_v<*pSubFcnType, mpi_comm_fcn_t>)
	{
		return [&vec](MPI_Comm comm)
		{
			MPIPropertyData vecData;
			for(auto &cEl : vec)
			{
				if constexpr (SEND)
				{	emplaceSingleObject(vecData, "", getMPIDataType<SEND>(cEl));	}
				else
				{	cEl = deserializeSingleProperty<PROPERTY>(vecData, "");	}

				assert(!vecData.ExchangeFunctions.empty());
				vecData.ExchangeFunctions.front()(comm);

				vecData.ExchangeFunctions.clear();
			}
		};
	}
	else
	{
		auto elementT = getMPIDataType<SEND>(vec.front());
		MPI_Datatype vecT;
		MPI_Type_contiguous(vec.size(), elementT, &vecT);

		MPI_Type_commit(&vecT);
		return std::tuple(MPIDerivedDatatype(vecT), getMPIAddr(vec.data()), vec.size());
	}
}

template<bool SEND, class PROPERTY, auto N>
auto ObjectPropertySerializerMethods<MPIPropertyData>::getMPIDataType(std::array<PROPERTY, N> &vec)
{
	static constexpr auto (*pSubFcnType)(PROPERTY&) = getMPIDataType<SEND>;
	if constexpr (std::is_same_v<*pSubFcnType, mpi_comm_fcn_t>)
	{
		return [&vec](MPI_Comm comm)
		{
			MPIPropertyData vecData;
			for(auto &cEl : vec)
			{
				cEl = deserializeSingleProperty<PROPERTY>(vecData, "");

				assert(!vecData.ExchangeFunctions.empty());
				vecData.ExchangeFunctions.front()(comm);

				vecData.ExchangeFunctions.clear();
			}
		};
	}
	else
	{
		auto elementT = getMPIDataType<SEND>(vec.front());
		MPI_Datatype vecT;
		MPI_Type_contiguous(vec.size(), elementT, &vecT);

		MPI_Type_commit(&vecT);
		return std::tuple(MPIDerivedDatatype(vecT), getMPIAddr(vec.data()), N);
	}
}

template<bool SEND, class VECTOR>
void ObjectPropertySerializerMethods<MPIPropertyData>::handleVectorFcn(MPI_Comm comm, VECTOR &vec)
{
	MPIPropertyData vecData;
	for(auto &cEl : vec)
	{
		if constexpr (SEND)
		{	emplaceSingleObject(vecData, "", getMPIDataType<SEND>(cEl));	}
		else
		{	cEl = deserializeSingleProperty<VECTOR::value_type>(vecData, "");	}

		assert(!vecData.ExchangeFunctions.empty());
		vecData.ExchangeFunctions.front()(comm);

		vecData.ExchangeFunctions.clear();
	}
}

#endif
