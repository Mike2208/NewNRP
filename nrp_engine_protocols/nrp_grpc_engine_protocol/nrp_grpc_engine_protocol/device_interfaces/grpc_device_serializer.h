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

#ifndef GRPC_DEVICE_SERIALIZER_H
#define GRPC_DEVICE_SERIALIZER_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/device_interface/device_serializer.h"

//#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.grpc.pb.h"
#include "nrp_grpc_engine_protocol/grpc_server/engine_grpc.pb.h"

class GRPCDevice
{
	public:
		GRPCDevice()
		{
			this->_dev.Clear();
		}

		template<class ...T>
		GRPCDevice(T &&...data)
		    : _dev(std::forward<T>(data)...)
		{}

		constexpr operator const auto&() const
		{	return this->_dev;	}

		constexpr operator auto&()
		{	return this->_dev;	}

		constexpr const auto &dev() const
		{	return this->_dev;	}

		constexpr auto &dev()
		{	return this->_dev;	}

	private:
		EngineGrpc::DeviceMessage _dev;
};

template<>
class DeviceSerializerMethods<GRPCDevice>
{
	public:
		using prop_deserialization_t = const EngineGrpc::DeviceMessage*;
		using deserialization_t = const EngineGrpc::DeviceMessage*;

		template<DEVICE_C DEVICE>
		static GRPCDevice serialize(const DEVICE &dev);

		template<class SERIALIZER_T = GRPCDevice>
		static SERIALIZER_T serializeID(const DeviceIdentifier &devID);

		template<DEVICE_C DEVICE>
		static DEVICE deserialize(DeviceIdentifier &&devID, deserialization_t data);

		static DeviceIdentifier deserializeID(deserialization_t data);
};

template<>
GRPCDevice DeviceSerializerMethods<GRPCDevice>::serializeID<GRPCDevice>(const DeviceIdentifier &devID);

template<>
EngineGrpc::DeviceMessage DeviceSerializerMethods<GRPCDevice>::serializeID<EngineGrpc::DeviceMessage>(const DeviceIdentifier &devID);

using GRPCDeviceSerializerMethods = DeviceSerializerMethods<GRPCDevice>;

#endif // GRPC_DEVICE_SERIALIZER_H
