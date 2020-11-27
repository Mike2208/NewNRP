#ifndef NEST_ENGINE_DEVICE_CONTROLLER_H
#define NEST_ENGINE_DEVICE_CONTROLLER_H

#include "nrp_nest_json_engine/devices/nest_device.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.h"
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
