#ifndef NRP_CAMERA_CONTROLLER_PLUGIN_H
#define NRP_CAMERA_CONTROLLER_PLUGIN_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"

#include "nrp_gazebo_device_interface/devices/json_physics_camera.h"

#include <gazebo/gazebo.hh>
#include <gazebo/sensors/CameraSensor.hh>
#include <gazebo/plugins/CameraPlugin.hh>

namespace gazebo
{
	class CameraDeviceController
	        : public EngineJSONDeviceController
	{
		public:
			CameraDeviceController(const rendering::CameraPtr &camera);
			virtual ~CameraDeviceController() override;

			virtual nlohmann::json getDeviceInformation(const nlohmann::json::const_iterator &data) override;
			virtual nlohmann::json handleDeviceData(const nlohmann::json &data) override;

		private:
			rendering::CameraPtr _camera;

			JSONPhysicsCamera _data;
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
