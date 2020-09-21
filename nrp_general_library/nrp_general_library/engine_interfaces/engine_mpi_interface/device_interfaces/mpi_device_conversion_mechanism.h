#ifndef MPI_DEVICE_CONVERSION_MECHANISM_H
#define MPI_DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

struct MPIDeviceData
        : public MPIPropertyData
{
	DeviceIdentifier DeviceID;
};

class MPICommunication
{
	public:
		static void sendMPIPropertyData(MPI_Comm comm, MPIPropertyData &dat);
		static void recvMPIPropertyData(MPI_Comm comm, MPIPropertyData &dat);

		template<DEVICE_C DEVICE>
		static void sendDevice(MPI_Comm comm, DEVICE &dev)
		{
			sendDeviceID(comm, dev.id());

			MPIPropertyData dat;
			MPIPropertySerializer<DEVICE>::serializeProperties(dev, dat);

			if(dat.VariableLengths.size() > 0)
			{	sendMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_UNSIGNED, 0, MPI_ANY_TAG, comm);	}

			sendMPI(MPI_BOTTOM, 1, dat.Datatype, 0, MPI_ANY_TAG, comm);
		}

		template<DEVICE_C DEVICE>
		static void recvDevice(MPI_Comm comm, DEVICE &dev)
		{
			dev.id() = recvDeviceID(comm);

			MPIPropertyData dat;
			dat.VariableLengths.resize(ObjectPropertySerializerMethods<MPIPropertyData>::getNumVariableProperties<DEVICE>());
			if(dat.VariableLengths.size() > 0)
			{	recvMPI(dat.VariableLengths.data(), dat.VariableLengths.size(), MPI_UNSIGNED, 0, MPI_ANY_TAG, comm);	}

			dat.CurVarLIt = dat.VariableLengths.begin();
			MPIPropertySerializer<DEVICE>::updateProperties(dat, dev);

			sendMPI(MPI_BOTTOM, 1, dat.Datatype, 0, MPI_ANY_TAG, comm);
		}

		static void sendDeviceID(MPI_Comm comm, const DeviceIdentifier &id);
		static DeviceIdentifier recvDeviceID(MPI_Comm comm);

	protected:
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
	{
		MPIPropertyData retVal;
		MPIPropertySerializer<DEVICE>::serializeProperties(device, retVal);
		return retVal;
	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsSerializable = requires(DEVICE &dev) { serialize(dev);	};

	static std::unique_ptr<DeviceInterface> deserialize(MPIDeviceData &data);


//	template<DEVICE_C DEVICE>
//	static DEVICE deserialize(MPIDeviceData &data)
//	{
//		data.CurVarLIt = data.VariableLengths.begin();
//		return DEVICE(data.DeviceID, data);
//	}

	template<DEVICE_C DEVICE>
	static constexpr bool IsDeserializable = requires(const DeviceIdentifier &devID, MPIPropertyData &data) {	DEVICE(devID, data);	};

	private:
};

template<DEVICE_C DEVICE, DEVICE_C ...DEVICES>
std::unique_ptr<DeviceInterface> DeviceConversionMechanism<MPIPropertyData, MPIDeviceData, DEVICE, DEVICES...>::deserialize
{

}

template<DEVICE_C ...DEVICES>
using MPIDeviceConversionMechanism = DeviceConversionMechanism<MPIPropertyData, MPIDeviceData, DEVICES...>;


#endif // MPI_DEVICE_CONVERSION_MECHANISM_H
