#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include "nrp_general_library/utils/concepts.h"
#include "nrp_general_library/utils/property_template.h"
#include "nrp_general_library/utils/ptr_templates.h"
#include "nrp_general_library/utils/serializers/property_serializer.h"

#include <concepts>
#include <string>
#include <string_view>
#include <memory>
#include <type_traits>

#include <nrp_grpc_library/engine_grpc.grpc.pb.h>

/*!
 * \brief Identifies a single device
 */
struct DeviceIdentifier
{
	/*!
	 * \brief Device Name. Used by simulator to identify source/sink of device
	 */
	std::string Name;

	/*!
	 * \brief Corresponding engine
	 */
	std::string EngineName;

	/*!
	 * \brief Device Type
	 */
	std::string Type;

	DeviceIdentifier() = default;

	DeviceIdentifier(const std::string &_name, const std::string &_engineName, const std::string &_type);

	template<std::constructible_from<std::string> STRING1_T, std::constructible_from<std::string> STRING2_T, std::constructible_from<std::string> STRING3_T>
	DeviceIdentifier(STRING1_T &&_name, STRING2_T &&_engineName, STRING3_T &&_type)
	    : Name(std::forward<STRING1_T>(_name)),
	      EngineName(std::forward<STRING2_T>(_engineName)),
	      Type(std::forward<STRING3_T>(_type))
	{}

	bool operator== (const DeviceIdentifier &) const = default;
	auto operator<=>(const DeviceIdentifier &) const = default;
};

template<class T>
concept DEV_ID_C = requires()
{	std::same_as<T, DeviceIdentifier>;	};

/*!
 * \brief Interface to devices, both for physics as well as brain simulators
 */
class DeviceInterface
        : public PtrTemplates<DeviceInterface>
{
	public:
		enum RESULT
		{ SUCCESS, ERROR };

		DeviceInterface() = default;

		template<DEV_ID_C DEV_ID_T>
		DeviceInterface(DEV_ID_T &&id)
		    : _id(std::forward<DEV_ID_T>(id))
		{}

		DeviceInterface(const std::string &name, const std::string &engineName, const std::string &type);
		virtual ~DeviceInterface() = default;

		const std::string &name() const;
		void setName(const std::string &name);

		const std::string &type() const;
		void setType(const std::string &type);

		const std::string &engineName() const;
		void setEngineName(const std::string &engineName);

		const DeviceIdentifier &id() const;
		void setID(const DeviceIdentifier &id);

		virtual void serialize(EngineGrpc::SetDeviceMessage * request) const;
		virtual void deserialize(const EngineGrpc::GetDeviceMessage &deviceData);


	private:
		/*!
		 * \brief Identifies Device. Contains name and type of this device
		 */
		DeviceIdentifier _id;
};

using DeviceInterfaceSharedPtr      = DeviceInterface::shared_ptr;
using DeviceInterfaceConstSharedPtr = DeviceInterface::const_shared_ptr;

template<class T>
concept DEVICE_C = requires {
        {	T::TypeName	};
        std::derived_from<T, DeviceInterface>;
        std::derived_from<T, PropertyTemplateGeneral>;
        std::constructible_from<T, DeviceIdentifier&&>;
        std::constructible_from<T, DeviceIdentifier&&, typename T::property_template_t &&>;
};

#endif // DEVICE_INTERFACE_H
