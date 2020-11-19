#include "nrp_camera_controller_plugin/nrp_camera_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

gazebo::CameraDeviceController::CameraDeviceController(const std::string &devName, const rendering::CameraPtr &camera, const sensors::SensorPtr &parent)
    : EngineGrpcDeviceController(DeviceIdentifier(devName, "", PhysicsCamera::TypeName.data())),
      _camera(camera),
      _parentSensor(parent),
      _data(camera->ScopedName())
{}

gazebo::CameraDeviceController::~CameraDeviceController() = default;

bool gazebo::CameraDeviceController::getData(EngineGrpc::GetDeviceMessage * reply)
{
	if(this->_newCamDat)
	{
		// If new data is available, send it
		reply->mutable_camera()->set_imageheight(this->_data.imageHeight());
		reply->mutable_camera()->set_imagewidth(this->_data.imageWidth());
		reply->mutable_camera()->set_imagedepth(this->_data.imagePixelSize());
		reply->mutable_camera()->set_imagedata(this->_imData);

		this->_newCamDat = false;
		return true;
	}
	else
	{
		// If no new data is available, send empty message
		reply->Clear();
		return false;
	}
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
		//std::cout << "Updating camera data\n";
		this->_lastSensorUpdateTime = sensorUpdateTime;

		// Set headers
		this->_data.setImageHeight(height);
		this->_data.setImageWidth(width);
		this->_data.setImagePixelSize(depth);

		const auto imageSize = width*height*depth;
		//this->_data.imageData().resize(imageSize);
		//memcpy(this->_data.imageData().data(), image, imageSize);

		this->_imData.resize(imageSize);
		memcpy(this->_imData.data(), image, imageSize);
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
