#ifndef ENGINE_GRPC_DEVICE_CONTROLLER_H
#define ENGINE_GRPC_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/device_interface.h"

struct DummyGrpcData
{
    DummyGrpcData(int a) : a(a) {};

    int a;
};

class EngineGrpcDeviceController
        : public DeviceIdentifier,
          public PtrTemplates<EngineGrpcDeviceController>
{
	public:
		/*!
		 * \brief Constructor
		 * \param id ID of device that is controlled by this class
		 */
		EngineGrpcDeviceController(const DeviceIdentifier &id);
		virtual ~EngineGrpcDeviceController();

		/*!
		 * \brief Get device information to be forwarded to the NRP
		 * \param data Additional data
		 * \return Returns a json structure containing device information
		 */
		virtual DummyGrpcData getDeviceInformation(const DummyGrpcData &data) = 0;

		/*!
		 * \brief Handle received device data
		 * \param data Data to be processed
		 * \return Returns result of handling device
		 */
		virtual DummyGrpcData handleDeviceData(const DummyGrpcData &data) = 0;
};

#endif // ENGINE_GRPC_DEVICE_CONTROLLER_H