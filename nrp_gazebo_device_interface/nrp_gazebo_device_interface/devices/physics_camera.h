#ifndef PHYSICS_CAMERA_H
#define PHYSICS_CAMERA_H

#include "nrp_general_library/device_interface/device_interface.h"
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

	static constexpr std::string_view TypeName = "p_camera";

	using JPropNames = PropNames<ImageHeight, ImageWidth, ImagePixelSize, ImageData>;
	using JProps = PropertyTemplate<PhysicsCamera, PhysicsCameraConst::JPropNames, uint32_t, uint32_t, uint8_t, std::vector<unsigned char> >;
};

/*!
 * \brief Physics Camera Image
 */
class PhysicsCamera
        : public PhysicsCameraConst,
          public DeviceInterface,
          public PhysicsCameraConst::JProps
{
	public:
		PhysicsCamera(const std::string &name);
		PhysicsCamera(const DeviceIdentifier &id);
		PhysicsCamera(const DeviceIdentifier &id, const nlohmann::json &data);
		PhysicsCamera(const DeviceIdentifier &id, const EngineGrpc::GetDeviceMessage &data);

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

		virtual void serialize(EngineGrpc::SetDeviceMessage * request) const override;
};

#endif // PHYSICS_CAMERA_H
