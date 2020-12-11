//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_camera_controller_plugin/nrp_camera_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

gazebo::CameraDeviceController::CameraDeviceController(const std::string &devName, const rendering::CameraPtr &camera, const sensors::SensorPtr &parent)
    : EngineMPIDeviceController(DeviceIdentifier(devName, "", PhysicsCamera::TypeName.data())),
      _camera(camera),
      _parentSensor(parent),
      _data(camera->ScopedName())
{}

gazebo::CameraDeviceController::~CameraDeviceController() = default;

MPIPropertyData gazebo::CameraDeviceController::getDeviceOutput()
{
	// Data updated via updateCamData()
	return MPIPropertySerializer<PhysicsCamera>::serializeProperties(this->_data);
}

void gazebo::CameraDeviceController::handleDeviceInput(PhysicsCamera &data)
{}

void gazebo::CameraDeviceController::updateCamData(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth)
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
	}
}

gazebo::NRPCameraController::~NRPCameraController() = default;

void gazebo::NRPCameraController::Load(gazebo::sensors::SensorPtr sensor, sdf::ElementPtr sdf)
{
	// Load camera plugin
	this->CameraPlugin::Load(sensor, sdf);

	const auto devName = NRPCommunicationController::createDeviceName(*this, sensor->Name());
	std::cout << "NRPCameraController: Registering new controller \"" << devName << "\"\n";

	// Create camera device and register it
	this->_cameraInterface.reset(new CameraDeviceController(devName, this->camera, sensor));
	NRPCommunicationController::getInstance().registerDeviceController(this->_cameraInterface.get());
}

void gazebo::NRPCameraController::OnNewFrame(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth, const std::string &)
{	this->_cameraInterface->updateCamData(image, width, height, depth);	}
