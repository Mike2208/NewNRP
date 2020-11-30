#ifndef PHYSICS_CAMERA_H
#define PHYSICS_CAMERA_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <vector>

class PhysicsCamera;

struct PhysicsCameraConst
{
	/*!
	 * \brief Image Height
	 */
	static constexpr FixedString ImageHeight = "im_height";

	/*!
	 * \brief Image Width
	 */
	static constexpr FixedString ImageWidth = "im_width";

	/*!
	 * \brief Image Pixel Size (in bytes)
	 */
	static constexpr FixedString ImagePixelSize = "im_depth";

	/*!
	 * \brief Image Data
	 */
	static constexpr FixedString ImageData = "im_data";

	using cam_data_t = std::vector<unsigned char>;

	using JPropNames = PropNames<ImageHeight, ImageWidth, ImagePixelSize, ImageData>;
};

/*!
 * \brief Physics Camera Image
 */
class PhysicsCamera
        : public PhysicsCameraConst,
          public Device<PhysicsCamera, "PhysicsCamera", PhysicsCameraConst::JPropNames, uint32_t, uint32_t, uint8_t, PhysicsCameraConst::cam_data_t>
{
	public:
		PhysicsCamera(DeviceIdentifier &&devID, property_template_t &&props = property_template_t(0, 0, 0, std::vector<unsigned char>({})))
		    : Device(std::move(devID), std::move(props))
		{}

		template<class DESERIALIZE_T>
		static auto deserializeProperties(DESERIALIZE_T &&data)
		{	return Device::deserializeProperties(std::forward<DESERIALIZE_T>(data), 0, 0, 0, std::vector<unsigned char>({}));	}

		PhysicsCamera(const std::string &name);
		PhysicsCamera(const DeviceIdentifier &id);
		PhysicsCamera(const DeviceIdentifier &id, const nlohmann::json &data);

		uint32_t imageHeight() const;
		uint32_t &imageHeight();
		void setImageHeight(uint32_t height);

		uint32_t imageWidth() const;
		uint32_t &imageWidth();
		void setImageWidth(uint32_t width);

		uint8_t imagePixelSize() const;
		uint8_t &imagePixelSize();
		void setImagePixelSize(uint8_t pixel_size);

		const cam_data_t &imageData() const;
		cam_data_t &imageData();
		void setImageData(const cam_data_t &imageData);
};

#endif // PHYSICS_CAMERA_H
