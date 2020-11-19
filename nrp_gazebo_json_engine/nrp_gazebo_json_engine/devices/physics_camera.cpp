#include "nrp_gazebo_json_engine/devices/physics_camera.h"

PhysicsCamera::PhysicsCamera(const std::string &name)
    : PhysicsCamera(DeviceIdentifier(name, "", TypeName.data()))
{}

PhysicsCamera::PhysicsCamera(const DeviceIdentifier &id)
    : DeviceInterface(id),
      PropertyTemplate(0, 0, 0, std::vector<unsigned char>({}))
{}

PhysicsCamera::PhysicsCamera(const DeviceIdentifier &id, const nlohmann::json &data)
    : DeviceInterface(id),
      PropertyTemplate(JSONPropertySerializer<PropertyTemplate>::readProperties(data, 0, 0, 0, std::vector<unsigned char>({})))
{}

uint32_t PhysicsCamera::imageHeight() const
{
	return this->getPropertyByName<ImageHeight>();
}

uint32_t &PhysicsCamera::imageHeight()
{
	return this->getPropertyByName<ImageHeight>();
}

void PhysicsCamera::setImageHeight(uint32_t height)
{
	this->getPropertyByName<ImageHeight>() = height;
}

uint32_t PhysicsCamera::imageWidth() const
{
	return this->getPropertyByName<ImageWidth>();
}

uint32_t &PhysicsCamera::imageWidth()
{
	return this->getPropertyByName<ImageWidth>();
}

void PhysicsCamera::setImageWidth(uint32_t width)
{
	this->getPropertyByName<ImageWidth>() = width;
}

uint8_t PhysicsCamera::imagePixelSize() const
{
	return this->getPropertyByName<ImagePixelSize>();
}

uint8_t &PhysicsCamera::imagePixelSize()
{
	return this->getPropertyByName<ImagePixelSize>();
}

void PhysicsCamera::setImagePixelSize(uint8_t pixel_size)
{
	this->getPropertyByName<ImagePixelSize>() = pixel_size;
}

const PhysicsCameraConst::cam_data_t &PhysicsCamera::imageData() const
{
	return this->getPropertyByName<ImageData>();
}

PhysicsCameraConst::cam_data_t &PhysicsCamera::imageData()
{
	return this->getPropertyByName<ImageData>();
}

void PhysicsCamera::setImageData(const cam_data_t &imageData)
{
	this->getPropertyByName<ImageData>() = imageData;
}
