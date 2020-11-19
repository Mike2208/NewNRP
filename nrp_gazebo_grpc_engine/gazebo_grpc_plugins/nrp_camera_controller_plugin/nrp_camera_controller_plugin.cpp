#include "nrp_camera_controller_plugin/nrp_camera_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

gazebo::CameraDeviceController::CameraDeviceController(const std::string &devName, const rendering::CameraPtr &camera, const sensors::SensorPtr &parent)
    : EngineGrpcDeviceController(DeviceIdentifier(devName, PhysicsCamera::TypeName.data(), "")),
      _camera(camera),
      _parentSensor(parent),
      _data(camera->ScopedName())
{}

gazebo::CameraDeviceController::~CameraDeviceController() = default;

void gazebo::CameraDeviceController::getData(EngineGrpc::GetDeviceMessage * reply)
{
	// Render image
	this->_camera->Render(true);

	reply->mutable_camera()->set_imageheight(this->_camera->ImageHeight());
	reply->mutable_camera()->set_imagewidth(this->_camera->ImageWidth());
	reply->mutable_camera()->set_imagedepth(this->_camera->ImageDepth());
	reply->mutable_camera()->set_imagedata(reinterpret_cast<char const *>(this->_camera->ImageData()));
}

void gazebo::CameraDeviceController::setData(const google::protobuf::Message & data)
{
	// Do nothing
}

void gazebo::CameraDeviceController::updateCamData(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth)
{
	const common::Time sensorUpdateTime = this->_parentSensor->LastMeasurementTime();

	if(sensorUpdateTime > this->_lastSensorUpdateTime)
	{
		std::cout << "Updating camera data\n";
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
	NRPCommunicationController::getInstance().registerDevice(devName, this->_cameraInterface.get());
}

void gazebo::NRPCameraController::OnNewFrame(const unsigned char *image, unsigned int width, unsigned int height, unsigned int depth, const std::string &)
{	this->_cameraInterface->updateCamData(image, width, height, depth);	}