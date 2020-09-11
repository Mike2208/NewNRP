#ifndef PYTHON_OBJECT_DEVICE_INTERFACE_H
#define PYTHON_OBJECT_DEVICE_INTERFACE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/serializers/json_property_serializer.h"

#include <boost/python.hpp>

struct PythonObjectDeviceInterfaceConst
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

class PythonObjectDeviceInterface
        : public DeviceInterface,
          public PropertyTemplate<PythonObjectDeviceInterface, PythonObjectDeviceInterfaceConst::PyPropNames, PythonObjectDeviceInterfaceConst::PyObjData>,
          public PythonObjectDeviceInterfaceConst
{
	public:
		static constexpr std::string_view TypeName = "py_obj";

		PythonObjectDeviceInterface(const DeviceIdentifier &id, const nlohmann::json &data);

		template<class ...DATA_T>
		PythonObjectDeviceInterface(const DeviceIdentifier &id, DATA_T &&...data)
		    : DeviceInterface(id),
		      PropertyTemplate(std::forward<DATA_T>(data)...)
		{}

		virtual ~PythonObjectDeviceInterface() override = default;

		const PyObjData &data() const;
		PyObjData &data();

		void setData(const PyObjData &data);

		boost::python::object pyData() const;
		void setPyData(boost::python::object data);
};

template<>
nlohmann::json JSONPropertySerializerMethods::serializeSingleProperty(const PythonObjectDeviceInterfaceConst::PyObjData &property);

template<>
PythonObjectDeviceInterfaceConst::PyObjData JSONPropertySerializerMethods::deserializeSingleProperty<PythonObjectDeviceInterfaceConst::PyObjData>(const nlohmann::json &data, const std::string_view &name);

#endif // PYTHON_OBJECT_DEVICE_INTERFACE_H
