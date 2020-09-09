#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include "nrp_general_library/utils/ptr_templates.h"

#include <string>
#include <string_view>
#include <memory>
#include <type_traits>

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
	 * \brief Device Type
	 */
	std::string Type;

	/*!
	 * \brief Corresponding engine
	 */
	std::string EngineName;

	DeviceIdentifier(const std::string &_name, const std::string &_type, const std::string &_engineName);

	bool operator==(const DeviceIdentifier &other) const;
	bool operator<(const DeviceIdentifier &other) const;
};

/*!
 * \brief Interface to devices, both for physics as well as brain simulators
 */
class DeviceInterface
        : public PtrTemplates<DeviceInterface>
{
	public:
		enum RESULT
		{ SUCCESS, ERROR };

		DeviceInterface(const DeviceIdentifier &id);
		DeviceInterface(const std::string &name, const std::string &type, const std::string &engineName);
		virtual ~DeviceInterface() = default;

		const std::string &name() const;
		void setName(const std::string &name);

		const std::string &type() const;
		void setType(const std::string &type);

		const std::string &engineName() const;
		void setEngineName(const std::string &engineName);

		const DeviceIdentifier &id() const;
		void setId(const DeviceIdentifier &id);

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
        (std::is_base_of_v<DeviceInterface, T>);
};

#endif // DEVICE_INTERFACE_H
