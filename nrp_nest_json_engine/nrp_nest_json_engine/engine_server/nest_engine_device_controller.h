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

#ifndef NEST_ENGINE_DEVICE_CONTROLLER_H
#define NEST_ENGINE_DEVICE_CONTROLLER_H

#include "nrp_nest_json_engine/devices/nest_device.h"
#include "nrp_json_engine_protocol/engine_server/engine_json_device_controller.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"
#include "nrp_general_library/utils/serializers/python_dict_property_serializer.h"

#include <boost/python.hpp>


template<DEVICE_C DEVICE>
class NestEngineJSONDeviceController;

template<>
class NestEngineJSONDeviceController<NestDevice>
        : public EngineJSONDeviceController<NestDevice>
{
	public:
		NestEngineJSONDeviceController(DeviceIdentifier &&devID, boost::python::object nodeCollection, boost::python::dict nest);

		virtual void handleDeviceDataCallback(NestDevice &&data) override;
		virtual const NestDevice *getDeviceInformationCallback() override;

        /*!
		 * \brief Set Nest properties
		 * \param nest Nest instance
		 * \param nodeCollection Nest GIDs of model managed by this controller
		 */
		void setNestID(boost::python::dict nest, boost::python::object nodeCollection);

	protected:
		/*!
		 * \brief Currently running Nest instance
		 */
		boost::python::dict _nest;

		/*!
		 * \brief Nest GID of model managed by this controller
		 */
		boost::python::object _nodeCollection;

		/*!
		 * \brief Device Data. Used to convert to/from JSON and python dict
		 */
		NestDevice _deviceData;

		/*!
		 * \brief Retrieves device status from Nest
		 * \return Returns dictionary containing device status
		 */
		boost::python::dict getStatusFromNest();

		/*!
		 * \brief Retrieves device status from Nest
		 * \param nest NEST object
		 * \param nodeCollection Nodes of device
		 * \return Returns dictionary with data
		 */
		static boost::python::dict getStatusFromNest(boost::python::dict &nest, const boost::python::object &nodeCollection);
};

#endif // NEST_ENGINE_DEVICE_CONTROLLER_H
