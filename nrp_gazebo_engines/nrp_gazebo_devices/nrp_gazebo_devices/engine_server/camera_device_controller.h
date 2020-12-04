#ifndef CAMERA_DEVICE_CONTROLLER_H
#define CAMERA_DEVICE_CONTROLLER_H

#include "nrp_general_library/engine_interfaces/engine_device_controller.h"

#include "nrp_gazebo_devices/physics_camera.h"

#include <gazebo/gazebo.hh>
#include <gazebo/sensors/CameraSensor.hh>
#include <gazebo/rendering/Camera.hh>

namespace gazebo
{
	template<class SERIALIZER>
	class CameraDeviceController
	        : public EngineDeviceController<SERIALIZER, PhysicsCamera>
	{
		public:
			CameraDeviceController(const std::string &devName, const rendering::CameraPtr &camera, const sensors::SensorPtr &parent)
			    : EngineDeviceController<SERIALIZER, PhysicsCamera>(PhysicsCamera::createID(devName, "")),
			      _parentSensor(parent),
			      _data(DeviceIdentifier(*this))
			{}

			virtual void handleDeviceDataCallback(PhysicsCamera &&data) override
			{}
			virtual const PhysicsCamera *getDeviceInformationCallback() override
			{
				if(this->_newDataAvailable)
				{
					this->_newDataAvailable = false;
					return &this->_data;
				}

				return nullptr;
			}

			void updateCamData(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth)
			{
				const common::Time sensorUpdateTime = this->_parentSensor->LastMeasurementTime();

				if(sensorUpdateTime > this->_lastSensorUpdateTime)
				{
					//std::cout << "Updating camera data\n";
					this->_lastSensorUpdateTime = sensorUpdateTime;

					// Set headers
					this->_data.setImageHeight(height);
					this->_data.setImageWidth(width);
					this->_data.setImagePixelSize(depth);

					const auto imageSize = width*height*depth;
					this->_data.imageData().resize(imageSize);
					memcpy(this->_data.imageData().data(), image, imageSize);

					this->_newDataAvailable = true;
				}
			}

		private:
			sensors::SensorPtr _parentSensor;

			common::Time _lastSensorUpdateTime = 0;

			PhysicsCamera _data;

			bool _newDataAvailable = true;
	};
}

#endif // CAMERA_DEVICE_CONTROLLER_H
