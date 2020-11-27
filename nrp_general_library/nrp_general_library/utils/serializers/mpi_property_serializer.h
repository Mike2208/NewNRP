#ifndef MPI_property_SERIALIZER_H
#define MPI_property_SERIALIZER_H

#include "nrp_general_library/device_interface/device_interface.h"
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
 * \brief MPIPropertyData with DeviceIdentifier. Used for deserialization if DevID is known
 */
struct MPIDeviceData
        : public MPIPropertyData
{
	DeviceIdentifier DeviceID;

	MPIDeviceData(DeviceIdentifier _deviceID, MPIPropertyData &&_dat = MPIPropertyData());
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
		using deserialization_t = MPIPropertyData&;

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

			if constexpr (SEND)
			{
				constexpr bool isResizable = requires ()
				{	MPISinglePropertySerializer<PROPERTY>::saveSize(data, *property);	};
				if constexpr (isResizable)
				{	MPISinglePropertySerializer<PROPERTY>::saveSize(data, *property);	}
			}

			constexpr auto serMethod = serializationMethod<PROPERTY>();
			if constexpr (serMethod == MPI_PROP_SERIALIZATION_BASE)
			{	data.addPropDatatype(MPISinglePropertySerializer<PROPERTY>::baseMPIDatatype(*property));	}
			else if constexpr (serMethod == MPI_PROP_SERIALIZATION_DERIVED)
			{	data.addPropDatatype(MPISinglePropertySerializer<PROPERTY>::derivedMPIDatatype(*property));	}
			else
			{
				static_assert(serMethod == MPI_PROP_SERIALIZATION_FCN, "Unknown serialization method specified");
				data.addPropDatatype(std::bind(&MPISinglePropertySerializer<PROPERTY>::template serializationFcn<SEND>, std::placeholders::_1, std::placeholders::_2, std::ref(*property)));
			}
		}

		/*!
		 * \brief Prepare single property for data receiving. Will resize vectors and other containers to proper size and insert corresponding datatype into data
		 * \param data MPI Data to determine property resize if required
		 * \return Returns a PROPERTY type of sufficient size to receive upcoming MPI_RECV
		 */
		template<class PROPERTY>
		static PROPERTY deserializeSingleProperty(MPIPropertyData &data, const std::string_view &)
		{
			return resizeIfVariable<PROPERTY>(data);
			//emplaceSingleObject<PROPERTY, false>(data, name, serializeSingleProperty(*pProp));
			//return *pProp;
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

#include "nrp_general_library/utils/serializers/mpi_property_serializer_methods.h"

template<PROPERTY_TEMPLATE_C PROPERTY_TEMPLATE>
struct PropertySerializer<MPIPropertyData, PROPERTY_TEMPLATE>
{
		using property_template_t = typename PROPERTY_TEMPLATE::property_template_t;

		/*!
		 *	\brief Update Properties by reading the given OBJECT. Will go through all existing properties and try to update them
		 *	\tparam OBJECT Data type to be deserialized
		 *	\param data OBJECT containing property data
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<MPIPropertyData> OBJECT_T>
		static void updateProperties(property_template_t &properties, OBJECT_T &&data)
		{
			PropertySerializerGeneral::template updateProperties<MPIPropertyData, property_template_t, OBJECT_T>(properties, std::forward<OBJECT_T>(data));

			if constexpr (property_template_t::NumProperties > 0)
			{	deserializeProperties<0>(properties, data);	}
		}

		/*!
		 *	\brief Read properties from the given OBJECT
		 *	\tparam OBJECT Data type to be deserialized
		 *	\tparam T Classes associated with properties. Per property, it should be string_view and PROPERTY
		 *	\param data OBJECT containing property data
		 *	\param defaultProperties Will be used if no corresponding value was found in config
		 */
		template<PROPERTY_SERIALIZER_OBJECT_C<MPIPropertyData> OBJECT_T, class ...T>
		static property_template_t readProperties(OBJECT_T &&data, T &&... defaultProperties)
		{
			if constexpr (property_template_t::NumProperties == 0)
			{
				return PropertySerializerGeneral::template deserializeObject<MPIPropertyData, property_template_t, OBJECT_T, T...>(
				            std::forward<OBJECT_T>(data), std::forward<T>(defaultProperties)...);
			}
			else
			{
				auto retVal = PropertySerializerGeneral::template deserializeObject<MPIPropertyData, property_template_t, OBJECT_T, T...>(
				            std::forward<OBJECT_T>(data), std::forward<T>(defaultProperties)...);

				deserializeProperties<0>(retVal, data);

				return retVal;
			}
		}

		/*!
		 *	\brief Serializes properties into OBJECT
		 *	\tparam OBJECT Data type to be serialized
		 *	\param properties Properties to convert
		 *	\param data OBJECT into which to insert the serialized data
		 *	\return Returns an OBJECT. For each property, the value will be stored under its given name
		 */
		template<class PROPERTY_TEMPLATE_T>
		static MPIPropertyData serializeProperties(PROPERTY_TEMPLATE_T &&properties, MPIPropertyData &&data = MPIPropertyData())
		{	return PropertySerializerGeneral::template serializeObject<MPIPropertyData, property_template_t>(std::forward<PROPERTY_TEMPLATE_T>(properties), std::move(data));	}

	private:
		using mpi_ser_t = ObjectPropertySerializerMethods<MPIPropertyData>;

		template<int ID>
		static inline void deserializeProperties(property_template_t &props, MPIPropertyData &data)
		{
			mpi_ser_t::emplaceSingleObject<typename property_template_t::template property_t<ID>, false>(data, "", mpi_ser_t::serializeSingleProperty(props.template getProperty<ID>()));

			if constexpr (ID+1 < property_template_t::NumProperties)
			{	return deserializeProperties<ID+1>(props, data);	}
		}
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
