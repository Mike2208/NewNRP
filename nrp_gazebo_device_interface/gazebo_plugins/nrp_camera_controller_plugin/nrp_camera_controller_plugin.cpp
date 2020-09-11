#include "nrp_camera_controller_plugin/nrp_camera_controller_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"

gazebo::CameraDeviceController::CameraDeviceController(const rendering::CameraPtr &camera)
    : EngineJSONDeviceController(DeviceIdentifier(camera->ScopedName(), PhysicsCamera::TypeName.data(), "")),
      _camera(camera),
      _data(camera->ScopedName())
{}

gazebo::CameraDeviceController::~CameraDeviceController() = default;

nlohmann::json gazebo::CameraDeviceController::getDeviceInformation(const nlohmann::json::const_iterator&)
{
	// Render image
	this->_camera->Render(true);

	// Set headers
	const auto imageHeight = this->_camera->ImageHeight();
	const auto imageWidth = this->_camera->ImageWidth();
	const auto imageDepth = this->_camera->ImageDepth();
	this->_data.setImageHeight(imageHeight);
	this->_data.setImageWidth(imageWidth);
	this->_data.setImagePixelSize(imageDepth);

	const auto imageSize = this->_camera->ImageByteSize();
	this->_data.imageData().resize(imageSize);
	memcpy(this->_data.imageData().data(), this->_camera->ImageData(), imageSize);

	// Save image data directly, prevents copying
	nlohmann::json retVal = JSONPropertySerializer<PhysicsCamera::property_template_t>::serializeProperties(this->_data, nlohmann::json());

	// Save image data
//	const unsigned char *img_data = this->_camera->ImageData();
//	const unsigned char *const img_data_end = img_data + this->_camera->ImageByteSize();
//	auto img = nlohmann::json::array();
//	for (; img_data < img_data_end; ++img_data)
//	{
//		img.push_back(*img_data);
//	}

//	retVal[PhysicsCamera::ImageData.m_data] = std::move(img);

	return retVal;
}

nlohmann::json gazebo::CameraDeviceController::handleDeviceData(const nlohmann::json &data)
{
	return nlohmann::json();
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
