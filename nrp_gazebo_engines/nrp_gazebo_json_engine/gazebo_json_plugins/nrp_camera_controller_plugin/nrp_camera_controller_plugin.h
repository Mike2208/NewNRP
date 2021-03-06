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

#ifndef NRP_CAMERA_CONTROLLER_PLUGIN_H
#define NRP_CAMERA_CONTROLLER_PLUGIN_H

#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"

#include "nrp_gazebo_devices/physics_camera.h"
#include "nrp_gazebo_devices/engine_server/camera_device_controller.h"

#include <gazebo/gazebo.hh>
#include <gazebo/sensors/CameraSensor.hh>
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
			std::unique_ptr<EngineJSONSerialization<CameraDeviceController> > _cameraInterface;
	};

	GZ_REGISTER_SENSOR_PLUGIN(NRPCameraController)
}

#endif
