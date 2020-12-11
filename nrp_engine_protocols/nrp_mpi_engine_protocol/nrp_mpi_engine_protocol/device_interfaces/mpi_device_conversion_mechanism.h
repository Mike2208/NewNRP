/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

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
