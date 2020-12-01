#ifndef NRP_CAMERA_CONTROLLER_PLUGIN_H
#define NRP_CAMERA_CONTROLLER_PLUGIN_H

#include "nrp_gazebo_devices/engine_server/camera_device_controller.h"
#include "nrp_gazebo_grpc_engine/devices/grpc_physics_camera.h"
#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"

#include <gazebo/plugins/CameraPlugin.hh>

namespace gazebo
{
	class NRPCameraController
	        : public CameraPlugin
	{
		public:
			virtual void Load(sensors::SensorPtr sensor, sdf::ElementPtr sdf) override;

			void OnNewFrame(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth, const std::string &format) override;

		private:
			std::unique_ptr<GrpcDeviceControlSerializer<CameraDeviceController> > _cameraInterface;
	};

	GZ_REGISTER_SENSOR_PLUGIN(NRPCameraController)
}

#endif
