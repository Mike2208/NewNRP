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

#ifndef ENGINE_DEVICE_CONTROLLER_H
#define ENGINE_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/device_interface/device_serializer.h"
#include "nrp_general_library/device_interface/device_serializer_methods.h"
#include "nrp_general_library/utils/function_traits.h"
#include "nrp_general_library/utils/ptr_templates.h"

/*!
 *  \brief Device Interface on the Server side. Handles IO for single device
 *  \tparam SERIALIZATION Serialization type
 */
template<class SERIALIZATION>
class EngineDeviceControllerInterface
        : public DeviceIdentifier,
          public PtrTemplates<EngineDeviceControllerInterface<SERIALIZATION> >
{
	public:
		using deserialization_t = typename DeviceSerializerMethods<SERIALIZATION>::deserialization_t;

		/*!
		 * \brief Constructor
		 * \param id ID of device that is controlled by this class
		 */
		EngineDeviceControllerInterface(DeviceIdentifier &&id)
		    : DeviceIdentifier(std::move(id))
		{}

		virtual ~EngineDeviceControllerInterface() = default;

		/*!
		 * \brief Get device information to be forwarded to the NRP
		 * \return Returns serialized data
		 */
		virtual SERIALIZATION getDeviceInformation() = 0;

		/*!
		 * \brief Handle received device data callback function
		 * \param data Data to be processed
		 */
		virtual void handleDeviceData(deserialization_t data) = 0;
};

template<class SERIALIZATION, DEVICE_C DEVICE>
class EngineDeviceController
        : public EngineDeviceControllerInterface<SERIALIZATION>
{
		using deserialization_t = typename EngineDeviceControllerInterface<SERIALIZATION>::deserialization_t;

	public:
		EngineDeviceController(DeviceIdentifier &&id)
		    : EngineDeviceControllerInterface<SERIALIZATION>(std::move(id))
		{}

		virtual SERIALIZATION getDeviceInformation() override final
		{
			const auto *const pDevDat = this->getDeviceInformationCallback();
			if(pDevDat != nullptr)
				return DeviceSerializerMethods<SERIALIZATION>::template serialize(*pDevDat);
			else
				return emptyValue();
		}

		virtual void handleDeviceData(deserialization_t data) override final
		{
			return this->handleDeviceDataCallback(DeviceSerializerMethods<SERIALIZATION>::template deserialize<DEVICE>(DeviceIdentifier(*this),
			                                                                                                           std::forward<deserialization_t>(data)));
		}

	protected:
		/*!
		 * \brief Handle received device data callback function
		 * \param data Data to be processed
		 */
		virtual void handleDeviceDataCallback(DEVICE &&data) = 0;

		/*!
		 * \brief Get device information to be forwarded to the NRP
		 * \return Returns a Device containing device information. If no new data available, return nullptr
		 */
		virtual const DEVICE *getDeviceInformationCallback() = 0;

		/*!
		 * \brief Get an empty value. This is used if getDeviceInformationCallback() has no new data available
		 */
		static SERIALIZATION emptyValue()
		{	return SERIALIZATION();	}
};


#endif // ENGINE_DEVICE_CONTROLLER_H
