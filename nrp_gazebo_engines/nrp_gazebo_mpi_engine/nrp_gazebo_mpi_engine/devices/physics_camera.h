/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef PHYSICS_CAMERA_H
#define PHYSICS_CAMERA_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/utils/property_template.h"

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

	using cam_data_t = std::vector<unsigned char>;

	using JPropNames = PropNames<ImageHeight, ImageWidth, ImagePixelSize, ImageData>;
	using JProps = PropertyTemplate<PhysicsCamera, PhysicsCameraConst::JPropNames, uint32_t, uint32_t, uint8_t, cam_data_t >;
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
		PhysicsCamera() = default;
		PhysicsCamera(const std::string &name);
		PhysicsCamera(const DeviceIdentifier &id);

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
