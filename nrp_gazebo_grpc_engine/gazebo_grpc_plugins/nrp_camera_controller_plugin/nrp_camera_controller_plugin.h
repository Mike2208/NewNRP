#ifndef NRP_CAMERA_CONTROLLER_PLUGIN_H
#define NRP_CAMERA_CONTROLLER_PLUGIN_H

#include <nrp_grpc_library/engine_grpc.grpc.pb.h>

#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_device_controller.h"

#include "nrp_gazebo_grpc_engine/devices/physics_camera.h"

#include <gazebo/gazebo.hh>
#include <gazebo/sensors/CameraSensor.hh>
#include <gazebo/plugins/CameraPlugin.hh>

namespace gazebo
{
	class CameraDeviceController
	        : public EngineGrpcDeviceController
	{
		public:
			CameraDeviceController(const std::string &devName, const rendering::CameraPtr &camera, const sensors::SensorPtr &parent);
			virtual ~CameraDeviceController() override;

			virtual bool getData(EngineGrpc::GetDeviceMessage * reply) override;
			virtual void setData(const google::protobuf::Message & data) override;

			void updateCamData(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth);

		private:
			rendering::CameraPtr _camera;

			sensors::SensorPtr _parentSensor;

			common::Time _lastSensorUpdateTime = 0;

			PhysicsCamera _data;
			std::string _imData;
			bool _newCamDat = true;
	};

	class NRPCameraController
	        : public CameraPlugin
	{
		public:
			virtual ~NRPCameraController();

			virtual void Load(sensors::SensorPtr sensor, sdf::ElementPtr sdf);

			void OnNewFrame(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth, const std::string &format) override;

		private:
			std::unique_ptr<CameraDeviceController> _cameraInterface;
	};

	GZ_REGISTER_SENSOR_PLUGIN(NRPCameraController)
}

#endif
