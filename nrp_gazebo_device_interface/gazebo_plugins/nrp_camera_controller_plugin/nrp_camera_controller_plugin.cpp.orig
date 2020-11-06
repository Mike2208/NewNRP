#include "nrp_camera_controller_plugin/nrp_camera_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

gazebo::CameraDeviceController::CameraDeviceController(const rendering::CameraPtr &camera)
    : EngineGrpcDeviceController(DeviceIdentifier(camera->ScopedName(), PhysicsCamera::TypeName.data(), "")),
      _camera(camera),
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

gazebo::NRPCameraController::~NRPCameraController() = default;

void gazebo::NRPCameraController::Load(gazebo::sensors::SensorPtr sensor, sdf::ElementPtr sdf)
{
	// Load camera plugin
	this->CameraPlugin::Load(sensor, sdf);

	const auto devName = NRPCommunicationController::createDeviceName(*this, sensor->Name());
	std::cout << "NRPCameraController: Registering new controller \"" << devName << "\"\n";

	// Create camera device and register it
	this->_cameraInterface.reset(new CameraDeviceController(this->camera));
	NRPCommunicationController::getInstance().registerDevice(devName, this->_cameraInterface.get());
}
