#ifndef MPI_property_SERIALIZER_H
#define MPI_property_SERIALIZER_H

#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <assert.h>
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


	/*!
	 * \brief MPI Datatype. Contains addresses under which property data can be retrieved
	 */
	MPIDerivedDatatype Datatype = nullptr;

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
	void addPropDatatype(MPIDerivedDatatype &&type, MPI_Aint address, int count);

	/*!
	 * \brief Add property data to relevant vectors
	 * \param type Property Type
	 * \param address Property Address (absolute)
	 * \param count Number of elements stored, usually 1
	 */
	void addPropDatatype(MPI_Datatype type, MPI_Aint address, int count);

	MPIPropertyData() = default;
	MPIPropertyData(unsigned int count, const int *dataCounts, const MPI_Aint *dataAddresses, const MPI_Datatype *datatypes);

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

template<>
class ObjectPropertySerializerMethods<MPIPropertyData>
        : public PropertySerializerGeneral
{
		enum MPI_PROP_SERIALIZATION_METHOD
		{	MPI_PROP_SERIALIZATION_BASE, MPI_PROP_SERIALIZATION_DERIVED, MPI_PROP_SERIALIZATION_FCN	};

		template<class PROPERTY>
		using ser_prop_t = std::tuple<const PROPERTY&, MPI_PROP_SERIALIZATION_METHOD>;

	public:
		using MPIDerivedDatatype = MPIPropertyData::MPIDerivedDatatype;
		using mpi_comm_fcn_t = MPIPropertyData::mpi_comm_fcn_t;

		template<class PROP_DATA_TYPE>
		using mpi_prop_datatype_t = MPIPropertyData::mpi_prop_datatype_t<PROP_DATA_TYPE>;

		using ObjectDeserializer = typename PropertySerializerGeneral::ObjectDeserializer<MPIPropertyData>;

		/*!
		 * \brief Only passes along property. Does nothing more
		 * \tparam PROPERTY Type to serialize
		 * \param property Data to serialize
		 * \return Returns ref to property
		 */
		template<class PROPERTY>
		static constexpr PROPERTY &serializeSingleProperty(PROPERTY &property)
		{	return property;	}

		template<class PROPERTY>
		static constexpr MPI_PROP_SERIALIZATION_METHOD serializationMethod();

		template<class PROPERTY>
		static mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype(PROPERTY &prop);

		template<class PROPERTY>
		static mpi_prop_datatype_t<MPIDerivedDatatype> derivedMPIDatatype(PROPERTY &prop);

		template<class PROPERTY, bool SEND>
		static void serializationFcn(MPI_Comm comm);


		/*!
		 * \brief Add single property information to data
		 * \tparam PROP_DATA Property information type. Can be either mpi_prop_datatype_t or mpi_comm_fcn_t
		 * \tparam SEND Should object be emplaced for sending (1) or receiving (0). Only changes the serializationFcn
		 * \param data Total PropertyTemplate information. singlePropData will be inserted here
		 * \param name Name of current property
		 * \param singlePropData Information about single property
		 */
		template<class PROPERTY, bool SEND = true>
		static void emplaceSingleObject(MPIPropertyData &data, const std::string_view &, PROPERTY &property)
		{
			constexpr auto serMethod = serializationMethod<PROPERTY>();
			if constexpr (serMethod == MPI_PROP_SERIALIZATION_BASE)
			{	data.addPropDatatype(baseMPIDatatype<PROPERTY>(property));	}
			else if constexpr (serMethod == MPI_PROP_SERIALIZATION_DERIVED)
			{	data.addPropDatatype(derivedMPIDatatype<PROPERTY>(property));	}
			else
			{
				static_assert(serMethod == MPI_PROP_SERIALIZATION_FCN, "Unknown serialization method specified");
				data.addPropDatatype(&serializationFcn<PROPERTY, SEND>);
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
			auto retVal = resizeIfVariableSize(PROPERTY(), data);

			emplaceSingleObject(data, name, retVal);

			return retVal;
		}

	private:

		/*!
		 * \brief Resize prop if it's
		 * \param prop Property to resize
		 * \param dat MPI Data. Contains size information for prop if it needs to be resized
		 * \return Returns modified prop
		 */
		template<class PROPERTY>
		static PROPERTY resizeIfVariableSize(PROPERTY &&prop, MPIPropertyData &)
		{	return prop;	}

		template<class PROPERTY>
		requires std::same_as<PROPERTY, std::vector<typename PROPERTY::element_type> > ||
		    std::same_as<PROPERTY, std::string>
		PROPERTY resizeIfVariableSize<PROPERTY>(PROPERTY &&prop, MPIPropertyData &dat)
		{
			assert(dat.CurVarLIt != dat.VariableLengths.end());

			// Resize vector and increment size iterator
			prop.resize(*(dat.CurVarLIt++));

			return std::move(prop);
		}

		static MPI_Aint getMPIAddr(void *loc)
		{
			MPI_Aint addr;
			MPI_Get_address(loc, &addr);

			return addr;
		}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<char>(char &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_CHAR, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<double>(double &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_DOUBLE, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<float>(float &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_FLOAT, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<int>(int &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_INT, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<long>(long &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_LONG, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<long double>(long double &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_LONG_DOUBLE, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<short>(short &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_SHORT, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<unsigned char>(unsigned char &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_CHAR, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<unsigned short>(unsigned short &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_SHORT, getMPIAddr(&dat), 1);	}

		template<>
		mpi_prop_datatype_t<MPI_Datatype> baseMPIDatatype<unsigned long>(unsigned long &dat)
		{	return mpi_prop_datatype_t<MPI_Datatype>(MPI_UNSIGNED_LONG, getMPIAddr(&dat), 1);	}

		template<class PROPERTY, bool SEND>
		requires
		void serializationFcn(MPI_Comm comm)
		{

		}

		/*!
		 * \brief Handles vector send/receive
		 * \tparam SEND Is this a Send or Receive operation?
		 * \tparam VECTOR Vector type. Should be compatible with std::vector<...>
		 * \param comm MPI Communication
		 * \param vec Vector to handle
		 */
		template<bool SEND, class VECTOR>
		static decltype(getMPIDataType<SEND>(std::declval<VECTOR::element_type>())) handleVector(VECTOR &vec)
		{
			constexpr auto (*pSubFcnType)(typename VECTOR::element_type&) = getMPIDataType<SEND>;
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
						{	cEl = deserializeSingleProperty<VECTOR::value_type>(vecData, "");	}

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
				MPI_Type_contiguous(vec.size(), std::get<0>(elementT), &vecT);

				MPI_Type_commit(&vecT);
				return std::tuple(MPIDerivedDatatype(vecT), getMPIAddr(vec.data()), 1);
			}
		}
};

template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
using MPIPropertySerializer = PropertySerializer<MPIPropertyData, PROPERTY_TEMPLATE>;

template<class PROPERTY>
constexpr ObjectPropertySerializerMethods<MPIPropertyData>::MPI_PROP_SERIALIZATION_METHOD ObjectPropertySerializerMethods<MPIPropertyData>::serializationMethod()
{
	if constexpr (std::is_invocable_v<decltype(&ObjectPropertySerializerMethods<MPIPropertyData>::serializationFcn<PROPERTY, true>), MPI_Comm>)
	{
		static_assert(std::is_invocable_v<decltype(&ObjectPropertySerializerMethods<MPIPropertyData>::serializationFcn<PROPERTY, false>), MPI_Comm>,
		        "Only found MPI serialization function for PROPERTY type, not deserialization fcn");
		return MPI_PROP_SERIALIZATION_FCN;
	}
	else if constexpr (std::is_invocable_v<decltype(&ObjectPropertySerializerMethods<MPIPropertyData>::baseMPIDatatype<PROPERTY>), PROPERTY&>)
	{	return MPI_PROP_SERIALIZATION_BASE;	}
	else
	{
		static_assert(std::is_invocable_v<decltype(&ObjectPropertySerializerMethods<MPIPropertyData>::derivedMPIDatatype<PROPERTY>), PROPERTY&>, "No proper serialization method found for this PROPERTY type");
		return MPI_PROP_SERIALIZATION_DERIVED;
	}
}

#endif
