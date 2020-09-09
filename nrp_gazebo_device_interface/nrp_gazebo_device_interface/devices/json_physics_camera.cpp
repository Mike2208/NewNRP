#include "nrp_gazebo_device_interface/devices/json_physics_camera.h"


JSONPhysicsCamera::JSONPhysicsCamera(const std::string &name)
    : JSONPhysicsCamera(DeviceIdentifier(name, TypeName.data(), ""))
{}

JSONPhysicsCamera::JSONPhysicsCamera(const DeviceIdentifier &id)
    : JSONDeviceInterface(id, 0, 0, 0, std::vector<unsigned char>({}))
{}

JSONPhysicsCamera::JSONPhysicsCamera(const DeviceIdentifier &id, const nlohmann::json &data)
    : JSONDeviceInterface(id, data, 0, 0, 0, std::vector<unsigned char>({}))
{}

uint32_t JSONPhysicsCamera::imageHeight() const
{
	return this->getPropertyByName<ImageHeight, uint32_t>();
}

uint32_t &JSONPhysicsCamera::imageHeight()
{
	return this->getPropertyByName<ImageHeight, uint32_t>();
}

void JSONPhysicsCamera::setImageHeight(uint32_t height)
{
	this->getPropertyByName<ImageHeight, uint32_t>() = height;
}

uint32_t JSONPhysicsCamera::imageWidth() const
{
	return this->getPropertyByName<ImageWidth, uint32_t>();
}

uint32_t &JSONPhysicsCamera::imageWidth()
{
	return this->getPropertyByName<ImageWidth, uint32_t>();
}

void JSONPhysicsCamera::setImageWidth(uint32_t width)
{
	this->getPropertyByName<ImageWidth, uint32_t>() = width;
}

uint8_t JSONPhysicsCamera::imagePixelSize() const
{
	return this->getPropertyByName<ImagePixelSize, uint8_t>();
}

uint8_t &JSONPhysicsCamera::imagePixelSize()
{
	return this->getPropertyByName<ImagePixelSize, uint8_t>();
}

void JSONPhysicsCamera::setImagePixelSize(uint8_t pixel_size)
{
	this->getPropertyByName<ImagePixelSize, uint8_t>() = pixel_size;
}

const std::vector<unsigned char> &JSONPhysicsCamera::imageData() const
{
	return this->getPropertyByName<ImageData, std::vector<unsigned char> >();
}

std::vector<unsigned char> &JSONPhysicsCamera::imageData()
{
	return this->getPropertyByName<ImageData, std::vector<unsigned char> >();
}

void JSONPhysicsCamera::setImageData(std::vector<unsigned char> &imageData)
{
	this->getPropertyByName<ImageData, std::vector<unsigned char> >() = imageData;
}
