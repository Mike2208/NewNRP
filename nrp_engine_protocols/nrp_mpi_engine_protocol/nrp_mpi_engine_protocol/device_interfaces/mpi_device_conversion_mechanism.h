#ifndef MPI_DEVICE_CONVERSION_MECHANISM_H
#define MPI_DEVICE_CONVERSION_MECHANISM_H

#include "nrp_general_library/device_interface/device_conversion_mechanism.h"
#include "nrp_general_library/utils/mpi_communication.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

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
		return MPICommunication::deserialize<DEVICE>(data);
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

#endif // MPI_DEVICE_CONVERSION_MECHANISM_H
