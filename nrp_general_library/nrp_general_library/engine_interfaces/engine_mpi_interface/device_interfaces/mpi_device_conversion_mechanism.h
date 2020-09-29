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
		static void sendMPIPropertyData(MPI_Comm comm, int tag, MPIPropertyData &dat)
		{
			if(dat.VariableLengths.size() > 0)
			{	sendMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_INT, 0, MPI_ANY_TAG, comm);	}

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

			MPIPropertyData dat;
			MPIPropertySerializer<DEVICE>::serializeProperties(dev, dat);

			sendPropertyTemplate<DEVICE>(comm, tag, dat);
		}

		template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVICES, bool SEND_ID = true>
		static inline void sendDeviceByType(MPI_Comm comm, int tag, DeviceInterface &devInterface);

		template<DEVICE_C DEVICE, bool RECV_ID = true>
		static void recvDevice(MPI_Comm comm, int tag, DEVICE &dev)
		{
			if constexpr (RECV_ID)
			{	dev.id() = recvDeviceID(comm, tag);	}

			recvPropertyTemplate<DEVICE>(comm, tag, dynamic_cast<DEVICE&>(dev));
		}

		template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVICES, bool RECV_ID = true>
		static inline DeviceInterface::unique_ptr recvDeviceByType(MPI_Comm comm, int tag, const DeviceIdentifier &devID)
		{
			MPIDeviceData dat(devID);
			return recvDeviceByType<DEVICE, REM_DEVICES..., RECV_ID>(comm, tag, dat);
		}

		template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVICES, bool RECV_ID = true>
		static inline DeviceInterface::unique_ptr recvDeviceByType(MPI_Comm comm, int tag, MPIDeviceData &devID);

		static void sendDeviceID(MPI_Comm comm, int tag, const DeviceIdentifier &id);
		static DeviceIdentifier recvDeviceID(MPI_Comm comm, int tag);

		static inline void sendMPI(const void *buff, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
		{
			try{MPI_Send(buff, count, datatype, dest, tag, comm);}
			catch(std::exception &e)
			{
				const auto errMsg = std::string("Error while sending MPI data: \n") + e.what();
				std::cerr << errMsg << std::endl;
				throw std::runtime_error(errMsg);
			}
		}

		static inline void recvMPI(void *buff, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm)
		{
			try{MPI_Recv(buff, count, datatype, source, tag, comm, nullptr);}
			catch(std::exception &e)
			{
				const auto errMsg = std::string("Error while receving MPI data: \n") + e.what();
				std::cerr << errMsg << std::endl;
				throw std::runtime_error(errMsg);
			}
		}
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
		MPIPropertySerializer<DEVICE>::updateProperties(*retVal, data);

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

template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVICES, bool SEND_ID>
inline void MPICommunication::sendDeviceByType(MPI_Comm comm, int tag, DeviceInterface &devInterface)
{
	if(devInterface.type() == DEVICE::TypeName)
	{	MPICommunication::sendDevice<DEVICE, SEND_ID>(comm, tag, devInterface);	}

	if constexpr (sizeof...(REM_DEVICES) > 0)
	{	return sendDeviceByType<REM_DEVICES..., SEND_ID>(comm, tag, devInterface);	}
	else
	{	throw std::domain_error("No device available for type \"" + devInterface.type() + "\"");	}
}

template<DEVICE_C DEVICE, DEVICE_C ...REM_DEVICES, bool RECV_ID>
DeviceInterface::unique_ptr MPICommunication::recvDeviceByType(MPI_Comm comm, int tag, MPIDeviceData &devID)
{
	if(devID.DeviceID.Type == DEVICE::TypeName)
	{
		auto retVal = MPIDeviceConversionMechanism<>::deserialize<DEVICE>(devID);

		MPICommunication::recvDevice<DEVICE, RECV_ID>(comm, tag, devID);

		return retVal;
	}

	if constexpr (sizeof... (REM_DEVICES) > 0)
	{	return recvDeviceByType<REM_DEVICES..., RECV_ID>(comm, tag, devID.DeviceID.Type);	}
	else
	{	throw std::domain_error("No device available for type \"" + devID.DeviceID.Type + "\"");	}
}


#endif // MPI_DEVICE_CONVERSION_MECHANISM_H
