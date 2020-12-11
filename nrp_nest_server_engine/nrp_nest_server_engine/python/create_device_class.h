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

#ifndef CREATE_DEVICE_CLASS_H
#define CREATE_DEVICE_CLASS_H

#include <memory>
#include <boost/python.hpp>

/*!
 * \brief Singleton class. Used to create Nest devices in a manner that makes them accessible to the NRP.
 */
class CreateDeviceClass
{
	public:
		/*!
		 * \brief Constructor
		 * \param nest Nest Dict
		 * \param devMap Device Mapping
		 */
		CreateDeviceClass(boost::python::dict nest, boost::python::dict devMap);

		/*!
		 * \brief Python function to create device and register it in _devMap
		 * \param args Python Args. args[0] is CreateDeviceClass, args[1] is the NRP device name. The remaining args will be passed to nest.Create(...)
		 * \param kwargs Python Keyword Args. Will be passed to nest.Create(...)
		 * \return Returns either Nest NodeCollection (Nest 3.x) or python::tuple of nest GIDs (Nest 2.x)
		 */
		static boost::python::object pyCreateDevice(boost::python::tuple args, boost::python::dict kwargs);

		/*!
		 * \brief pyRegisterDevice
		 * \param args Python Args. args[0] is CreateDeviceClass, args[1] is the NRP device name, args[2] is a NodeCollection object
		 * \param kwargs Python Kwargs. Not used
		 */
		static boost::python::object pyRegisterDevice(boost::python::tuple args, boost::python::dict kwargs);

		/*!
		 * \brief Create new Nest device and add it to devMap
		 * \param args Python Args. First argument must be string with device label. This will be used to reference the device from the NRP side. All other args will be passed to nest.Create(...)
		 * \param kwargs Python Keyword Args. Will be passed to nest.Create(...)
		 * \return Returns either Nest NodeCollection (Nest 3.x) or python::tuple of nest GIDs (Nest 2.x)
		 */
		boost::python::object createAndRegisterDevice(boost::python::tuple args, boost::python::dict kwargs);

		/*!
		 * \brief Register an existing device
		 * \param devName NRP Device name
		 * \param nodeCollection NodeCollection of device data
		 */
		void registerDevice(boost::python::str devName, boost::python::object nodeCollection);

		/*!
		 * \brief Python Call Function to get NRP device mapping
		 * \return Returns dict with device mapping
		 */
		boost::python::dict pyDevMap();

		/*!
		 * \brief Get device map
		 * \return Returns device map
		 */
		const boost::python::dict &devMap() const;

	private:
		/*!
		 * \brief Nest Module
		 */
		boost::python::dict _nest;

		/*!
		 * \brief Mapping from python::str to either Nest NodeCollection (Nest 3.x) or python::tuple of nest GIDs (Nest 2.x).
		 */
		boost::python::dict _devMap;
};

#endif // CREATE_DEVICE_CLASS_H
