#ifndef MPI_DEVICE_CONVERSION_MECHANISM_H
#define MPI_DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

struct MPIDeviceData
        : public MPIPropertyData
{
	DeviceIdentifier DeviceID;

	MPIDeviceData(DeviceIdentifier _deviceID, MPIPropertyData &&_dat = MPIPropertyData());
};

class MPICommunication
{
	public:
		static void sendString(MPI_Comm comm, int tag, const std::string &str);
		static std::string recvString(MPI_Comm comm, int tag);

		static void sendMPIPropertyData(MPI_Comm comm, int tag, MPIPropertyData &dat)
		{
			if(dat.VariableLengths.size() > 0)
			{	sendMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_INT, 0, tag, comm);	}

			sendMPI(MPI_BOTTOM, 1, dat.Datatype, 0, tag, comm);

			for(const auto &exchangeFcn : dat.ExchangeFunctions)
			{	exchangeFcn(comm, tag);	}
		}

		template<class PROPERTY_TEMPLATE>
		static void sendPropertyTemplate(MPI_Comm comm, int tag, PROPERTY_TEMPLATE &props)
		{
			MPIPropertyData dat = MPIPropertySerializer<PROPERTY_TEMPLATE>::serializeProperties(props);
			sendMPIPropertyData(comm, tag, dat);
		}

		template<class PROPERTY_TEMPLATE>
		static void recvPropertyTemplate(MPI_Comm comm, int tag, PROPERTY_TEMPLATE &props)
		{
			MPIPropertyData dat;
			dat.VariableLengths.resize(ObjectPropertySerializerMethods<MPIPropertyData>::getNumVariableProperties<PROPERTY_TEMPLATE>());
			if(dat.VariableLengths.size() > 0)
			{	recvMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_INT, 0, tag, comm);	}

			dat.CurVarLIt = dat.VariableLengths.begin();
			MPIPropertySerializer<PROPERTY_TEMPLATE>::updateProperties(props, dat);

			recvMPI(MPI_BOTTOM, 1, dat.Datatype, 0, tag, comm);

			for(const auto &exFcn : dat.ExchangeFunctions)
			{	exFcn(comm, tag);	}
		}

		template<DEVICE_C DEVICE, bool SEND_ID = true>
		static void sendDevice(MPI_Comm comm, int tag, DEVICE &dev)
		{
			if constexpr (SEND_ID)
			{	sendDeviceID(comm, tag, dev.id());	}

			MPICommunication::sendPropertyTemplate<DEVICE>(comm, tag, dev);
		}

		template<bool SEND_ID, DEVICE_C ...DEVICES>
		static void sendDeviceByType(MPI_Comm comm, int tag, DeviceInterface &devInterface)
		{
			if constexpr (sizeof... (DEVICES) == 0)
			{	throwNoDevError(devInterface.id());	}
			else
			{	return sendDeviceByTypeInternal<SEND_ID, DEVICES...>(comm, tag, devInterface);	}
		}

		template<DEVICE_C DEVICE, bool RECV_ID = true>
		static void recvDevice(MPI_Comm comm, int tag, DEVICE &dev)
		{
			if constexpr (RECV_ID)
			{	dev.id() = recvDeviceID(comm, tag);	}

			MPICommunication::recvPropertyTemplate<DEVICE>(comm, tag, dynamic_cast<DEVICE&>(dev));
		}

		template<bool RECV_ID, DEVICE_C ...DEVICES>
		static DeviceInterface::unique_ptr recvDeviceByType(MPI_Comm comm, int tag, const DeviceIdentifier &devID)
		{
			MPIDeviceData dat(devID);
			return recvDeviceByType<RECV_ID, DEVICES...>(comm, tag, dat);
		}

		template<bool RECV_ID, DEVICE_C ...DEVICES>
		static DeviceInterface::unique_ptr recvDeviceByType(MPI_Comm comm, int tag, MPIDeviceData &devData)
		{
			if constexpr (sizeof... (DEVICES) == 0)
			{	throwNoDevError(devData.DeviceID);	}
			else
			{	return recvDeviceByTypeInternal<RECV_ID, DEVICES...>(comm, tag, devData);	}
		}

		static void sendDeviceID(MPI_Comm comm, int tag, const DeviceIdentifier &id);
		static DeviceIdentifier recvDeviceID(MPI_Comm comm, int tag);

		static void sendMPI(const void *buff, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
		static void recvMPI(void *buff, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm);

	private:
		template<bool SEND_ID, DEVICE_C DEVICE, class ...REM_DEVICES>
		static inline void sendDeviceByTypeInternal(MPI_Comm comm, int tag, DeviceInterface &devInterface);

		template<bool RECV_ID, DEVICE_C DEVICE, class ...REM_DEVICES>
		static inline DeviceInterface::unique_ptr recvDeviceByTypeInternal(MPI_Comm comm, int tag, MPIDeviceData &devData);

		static inline void throwNoDevError(const DeviceIdentifier &devID)
		{	throw std::domain_error("No device available for type \"" + devID.Type + "\"");	}
};

template<DEVICE_C ...DEVICES>
struct DeviceConversionMechanism<MPIPropertyData, MPIDeviceData, DEVICES...> : protected MPICommunication
{
	static void init(const EngineInterface &)
	{}

	static void shutdown()
	{}

	template<DEVICE_C DEVICE>
	static MPIPropertyData serialize(DEVICE &device)
	{	return MPIPropertySerializer<DEVICE>::serializeProperties(device);	}

	static MPIPropertyData serialize(DeviceInterface &device)
	{	return trySerialize<DEVICES...>(device);	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsSerializable = requires(DEVICE &dev) { serialize(dev);	};

	template<DEVICE_C DEVICE>
	static typename PtrTemplates<DEVICE>::unique_ptr deserialize(MPIDeviceData &data)
	{
		typename PtrTemplates<DEVICE>::unique_ptr retVal(new DEVICE(data.DeviceID));
		MPIPropertySerializer<DEVICE>::updateProperties(*retVal, static_cast<MPIPropertyData&>(data));

		return retVal;
	}

	static DeviceInterface::unique_ptr deserialize(MPIDeviceData &data)
	{	return tryDeserialize<DEVICES...>(data);	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsDeserializable = requires(const DeviceIdentifier &devID, MPIPropertyData &data) {	DEVICE(devID, data);	};

	private:

	    template<DEVICE_C ...REM_DEVS>
	    static inline DeviceInterface::unique_ptr tryDeserialize(MPIDeviceData &data)
		{	throw std::domain_error("No device available for type \"" + data.DeviceID.Type + "\"\n");	}

		template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVS>
		static inline DeviceInterface::unique_ptr tryDeserialize(MPIDeviceData &data)
		{
			if (DEVICE::TypeName == data.DeviceID.Type)
				return deserialize<DEVICE>(data);

			return tryDeserialize<REM_DEVS...>(data);
		}

		template<DEVICE_C ...REM_DEVS>
		static inline MPIPropertyData trySerialize(DeviceInterface &data)
		{	throw std::domain_error("No device available for type \"" + data.type() + "\"\n");	}

		template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVS>
		static inline MPIPropertyData trySerialize(DeviceInterface &data)
		{
			if (DEVICE::TypeName == data.type())
				return serialize<DEVICE>(dynamic_cast<DEVICE&>(data));

			return trySerialize<REM_DEVS...>(data);
		}
};

template<DEVICE_C ...DEVICES>
using MPIDeviceConversionMechanism = DeviceConversionMechanism<MPIPropertyData, MPIDeviceData, DEVICES...>;

template<bool SEND_ID, DEVICE_C DEVICE, class ...REM_DEVICES>
void MPICommunication::sendDeviceByTypeInternal(MPI_Comm comm, int tag, DeviceInterface &devInterface)
{
	if(devInterface.type() == DEVICE::TypeName)
	{	MPICommunication::sendDevice<DEVICE, SEND_ID>(comm, tag, dynamic_cast<DEVICE&>(devInterface));	}

	if constexpr (sizeof...(REM_DEVICES) > 0)
	{	return sendDeviceByTypeInternal<SEND_ID, REM_DEVICES...>(comm, tag, devInterface);	}
	else
	{	throwNoDevError(devInterface.id());	}
}

template<bool RECV_ID, DEVICE_C DEVICE, class ...REM_DEVICES>
inline DeviceInterface::unique_ptr MPICommunication::recvDeviceByTypeInternal(MPI_Comm comm, int tag, MPIDeviceData &devData)
{
	if(devData.DeviceID.Type == DEVICE::TypeName)
	{
		auto retVal = MPIDeviceConversionMechanism<>::deserialize<DEVICE>(devData);

		MPICommunication::recvDevice<DEVICE, RECV_ID>(comm, tag, *retVal);

		return retVal;
	}

	if constexpr (sizeof... (REM_DEVICES) > 0)
	{	return recvDeviceByTypeInternal<RECV_ID, REM_DEVICES...>(comm, tag, devData);	}
	else
	{
		throwNoDevError(devData.DeviceID);
		return nullptr;		// Silence compiler no-return warning
	}
}

#endif // MPI_DEVICE_CONVERSION_MECHANISM_H
