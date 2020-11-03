#ifndef NRP_CAMERA_CONTROLLER_PLUGIN_H
#define NRP_CAMERA_CONTROLLER_PLUGIN_H

#include <engine_grpc.grpc.pb.h>

#include "nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.h"

#include "nrp_gazebo_device_interface/devices/physics_camera.h"

#include <gazebo/gazebo.hh>
#include <gazebo/sensors/CameraSensor.hh>
#include <gazebo/plugins/CameraPlugin.hh>

namespace gazebo
{
	class CameraDeviceController
	        : public EngineGrpcDeviceController
	{
		public:
			CameraDeviceController(const rendering::CameraPtr &camera);
			virtual ~CameraDeviceController() override;

			virtual const google::protobuf::Message * getData() override;
			virtual void setData(const google::protobuf::Message & data) override;

		private:
			rendering::CameraPtr _camera;

			PhysicsCamera _data;
	};

	class NRPCameraController
	        : public CameraPlugin
	{
		public:
			virtual ~NRPCameraController();

			virtual void Load(sensors::SensorPtr sensor, sdf::ElementPtr sdf);

		private:
			std::unique_ptr<CameraDeviceController> _cameraInterface;
	};

	GZ_REGISTER_SENSOR_PLUGIN(NRPCameraController)
}

#endif
