#include "nrp_camera_controller_plugin/nrp_camera_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

gazebo::CameraDeviceController::CameraDeviceController(const rendering::CameraPtr &camera)
    : EngineGrpcDeviceController(DeviceIdentifier(camera->ScopedName(), PhysicsCamera::TypeName.data(), "")),
      _camera(camera),
      _data(camera->ScopedName())
{}

gazebo::CameraDeviceController::~CameraDeviceController() = default;

const google::protobuf::Message * gazebo::CameraDeviceController::getData()
{
	// Render image
	this->_camera->Render(true);

	EngineGrpc::GazeboCamera * camera = new EngineGrpc::GazeboCamera();

	camera->set_imageheight(this->_camera->ImageHeight());
	camera->set_imagewidth(this->_camera->ImageWidth());
	camera->set_imagedepth(this->_camera->ImageDepth());
	camera->set_imagedata(reinterpret_cast<char const *>(this->_camera->ImageData()));

	return camera;
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
