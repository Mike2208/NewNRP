#ifndef NRP_CAMERA_CONTROLLER_PLUGIN_H
#define NRP_CAMERA_CONTROLLER_PLUGIN_H

#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"

#include "nrp_gazebo_json_engine/devices/physics_camera.h"

#include <gazebo/gazebo.hh>
#include <gazebo/sensors/CameraSensor.hh>
#include <gazebo/plugins/CameraPlugin.hh>

namespace gazebo
{
	class CameraDeviceController
	        : public EngineJSONDeviceController<PhysicsCamera>
	{
		public:
			CameraDeviceController(const std::string &devName, const rendering::CameraPtr &camera, const sensors::SensorPtr &parent);

			virtual void handleDeviceDataCallback(PhysicsCamera &&data) override;
			virtual const PhysicsCamera *getDeviceInformationCallback() override;

			void updateCamData(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth);

		private:
			rendering::CameraPtr _camera;

			sensors::SensorPtr _parentSensor;

			common::Time _lastSensorUpdateTime = 0;

			PhysicsCamera _data;

			bool _newDataAvailable = true;
	};

	class NRPCameraController
	        : public CameraPlugin
	{
		public:
			virtual void Load(sensors::SensorPtr sensor, sdf::ElementPtr sdf) override;

			void OnNewFrame(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth, const std::string &format) override;

		private:
			std::unique_ptr<CameraDeviceController> _cameraInterface;
	};

	GZ_REGISTER_SENSOR_PLUGIN(NRPCameraController)
}

#endif
