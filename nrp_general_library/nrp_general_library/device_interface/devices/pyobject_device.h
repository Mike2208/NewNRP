#ifndef PYOBJECT_DEVICE_H
#define PYOBJECT_DEVICE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <boost/python.hpp>

struct PyObjectDeviceConst
{
	struct PyObjData
	{
		std::string SerializedData;
		boost::python::object Data;

		boost::python::object JsonEncoder;
		boost::python::object JsonDecoder;

		PyObjData() = default;
		PyObjData(std::string _serializedData, boost::python::object _data, boost::python::object _jsonEncoder = PyObjData::defaultEncoder(), boost::python::object _jsonDecoder = PyObjData::defaultDecoder());
		PyObjData(std::string _serializedData);
		PyObjData(boost::python::object _data, boost::python::object _jsonEncoder = PyObjData::defaultEncoder(), boost::python::object _jsonDecoder = PyObjData::defaultDecoder());

		const std::string &serialize() const;
		boost::python::object deserialize() const;

		private:
		    static boost::python::object defaultEncoder();
			static boost::python::object defaultDecoder();
	};

	static constexpr FixedString Object = "object";

	using PyPropNames = PropNames<Object>;
};

class PyObjectDevice
        : public Device<PyObjectDevice, "py_obj", PyObjectDeviceConst::PyPropNames,
                        PyObjectDeviceConst::PyObjData>,
          public PyObjectDeviceConst
{
	public:
		PyObjectDevice(const DeviceIdentifier &id, const nlohmann::json &data);

		template<class DEVID_T, class ...DATA_T>
		PyObjectDevice(DEVID_T &&id, DATA_T &&...data)
		    : Device(std::forward<DEVID_T>(id), std::forward<DATA_T>(data)...)
		{}

		virtual ~PyObjectDevice() override = default;

		const PyObjData &data() const;
		PyObjData &data();

		void setData(const PyObjData &data);

		boost::python::object pyData() const;
		void setPyData(boost::python::object data);
};

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PyObjectDeviceConst::PyObjData &property);

template<>
PyObjectDeviceConst::PyObjData JSONPropertySerializerMethods::deserializeSingleProperty<PyObjectDeviceConst::PyObjData>(const nlohmann::json &data, const std::string_view &name);

#endif // PYOBJECT_DEVICE_H
