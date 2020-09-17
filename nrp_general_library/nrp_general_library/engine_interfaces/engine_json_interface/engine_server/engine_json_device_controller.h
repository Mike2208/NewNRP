#ifndef ENGINE_JSON_DEVICE_CONTROLLER_H
#define ENGINE_JSON_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/utils/ptr_templates.h"

#include <nlohmann/json.hpp>

/*! \addtogroup json_engine
 *  \brief Device Interface on the Server side. Handles IO for single device.
 */
class EngineJSONDeviceController
        : public DeviceIdentifier,
          public PtrTemplates<EngineJSONDeviceController>
{
	public:
		/*!
		 * \brief Constructor
		 * \param id ID of device that is controlled by this class
		 */
		EngineJSONDeviceController(const DeviceIdentifier &id);
		virtual ~EngineJSONDeviceController();

		/*!
		 * \brief Get device information to be forwarded to the NRP
		 * \param data Additional data
		 * \return Returns a json structure containing device information
		 */
		virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator &data) = 0;

		/*!
		 * \brief Handle received device data
		 * \param data Data to be processed
		 * \return Returns result of handling device
		 */
		virtual nlohmann::json handleDeviceData(const nlohmann::json &data) = 0;
};

using EngineJSONDeviceControllerSharedPtr = EngineJSONDeviceController::shared_ptr;
using EngineJSONDeviceControllerConstSharedPtr = EngineJSONDeviceController::const_shared_ptr;

#endif // ENGINE_JSON_DEVICE_CONTROLLER_H
