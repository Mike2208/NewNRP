#ifndef JSON_PHYSICS_CAMERA_H
#define JSON_PHYSICS_CAMERA_H

#include "nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_interface.h"

struct JSONPhysicsCameraConst
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

	static constexpr std::string_view TypeName = "p_camera";

	using JPropNames = PropNames<ImageHeight, ImageWidth, ImagePixelSize, ImageData>;
};

/*!
 * \brief Physics Camera Image
 */
class JSONPhysicsCamera
        : public JSONPhysicsCameraConst,
          public JSONDeviceInterface<JSONPhysicsCamera, JSONPhysicsCameraConst::JPropNames, uint32_t, uint32_t, uint8_t, std::vector<unsigned char> >
{
	public:
		JSONPhysicsCamera(const std::string &name);
		JSONPhysicsCamera(const DeviceIdentifier &id);
		JSONPhysicsCamera(const DeviceIdentifier &id, const nlohmann::json &data);

		uint32_t imageHeight() const;
		uint32_t &imageHeight();
		void setImageHeight(uint32_t height);

		uint32_t imageWidth() const;
		uint32_t &imageWidth();
		void setImageWidth(uint32_t width);

		uint8_t imagePixelSize() const;
		uint8_t &imagePixelSize();
		void setImagePixelSize(uint8_t pixel_size);

		const std::vector<unsigned char> &imageData() const;
		std::vector<unsigned char> &imageData();
		void setImageData(std::vector<unsigned char> &imageData);
};

#endif // JSON_PHYSICS_CAMERA_H
