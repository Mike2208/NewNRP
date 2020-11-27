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

		virtual void handleDeviceData(deserialization_t &&data) override final
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
