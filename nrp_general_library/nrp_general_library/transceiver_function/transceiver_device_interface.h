#ifndef TRANSCEIVER_DEVICE_INTERFACE_H
#define TRANSCEIVER_DEVICE_INTERFACE_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/utils/ptr_templates.h"

//#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"

#include <boost/python.hpp>

class TransceiverFunctionInterpreter;
using TransceiverFunctionInterpreterSharedPtr = std::shared_ptr<TransceiverFunctionInterpreter>;
using TransceiverFunctionInterpreterConstSharedPtr = std::shared_ptr<const TransceiverFunctionInterpreter>;

class TransceiverDeviceInterface;

/*!
 *	\brief Concept defining all TransceiverInterfaces
 */
template<class T>
concept TRANSCEIVER_DEVICE_C = std::is_base_of_v<TransceiverDeviceInterface, T> || std::is_same_v<TransceiverDeviceInterface, T>;

/*!
 * \brief Base of TF Decorators.
 */
class TransceiverDeviceInterface
        : public PtrTemplates<TransceiverDeviceInterface>
{
	public:
		TransceiverDeviceInterface() = default;
		virtual ~TransceiverDeviceInterface() = default;

		/*!
		 * \brief Decorator __call__() function. Takes the lower decorator as a parameter. Moves the given class into a shared_ptr, which will be managed by the next decorator
		 * \param tfDevice Lower Decorator
		 * \return shared_ptr referencing data from this object
		 */
		template<TRANSCEIVER_DEVICE_C CLASS>
		TransceiverDeviceInterface::shared_ptr pySetup(const TransceiverDeviceInterface::shared_ptr &tfDevice)
		{
			this->_function = tfDevice;

			auto thisPtr = this->moveToSharedPtr<CLASS>();

			TransceiverDeviceInterface::shared_ptr *const registryPtr = this->_function->getTFInterpreterRegistry();
			assert(registryPtr != nullptr);

			*registryPtr = thisPtr;
			return std::move(thisPtr);
		}

		/*!
		 * \brief Get name of engine this transceiver is linked to
		 */
		virtual const std::string &linkedEngineName() const;

		/*!
		 * \brief Execute Transceiver Function. Base class will simply call runTf on _function
		 * \param args Arguments for execution. Can be altered by any TransceiverDeviceInterfaces. Base class will only pass them along
		 * \param kwargs Keyword arguments for execution. Can be altered by any TransceiverDeviceInterfaces. Base class will only pass them along
		 * \return Returns result of TransceiverFunction execution.
		 */
		virtual boost::python::object runTf(boost::python::tuple &args, boost::python::dict &kwargs);

		/*!
		 *	\brief Appends its own device requests onto deviceIDs. Uses getRequestedDeviceIDs to check which IDs are requested by this device
		 *	\param deviceIDs Container with device IDs that gets expanded
		 *	\return Returns deviceIDs, with own deviceIDs appended
		 */
		virtual EngineInterface::device_identifiers_t updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs = EngineInterface::device_identifiers_t()) const;

		/*!
		 * \brief Returns device IDs of this Device that should be requested from the engines. TODO: Make protected
		 */
		virtual EngineInterface::device_identifiers_t getRequestedDeviceIDs() const;

		/*!
		 * \brief Set global TF Interpreter. All Transceiver Functions will register themselves with it upon creation
		 * \param interpreter Interpreter to use
		 */
		static void setTFInterpreter(TransceiverFunctionInterpreter *interpreter);

	protected:
		/*!
		 * \brief Pointer to TF Interpreter. Will be used to register a new TF function
		 */
		static TransceiverFunctionInterpreter *TFInterpreter;

		template<TRANSCEIVER_DEVICE_C CLASS>
		typename PtrTemplates<CLASS>::shared_ptr moveToSharedPtr()
		{	return typename PtrTemplates<CLASS>::shared_ptr(new CLASS(std::move(static_cast<CLASS&>(*this))));	}

		/*!
		 * \brief Gets address of ptr under which the TFInterpreter has registered this TF. Mainly used during setup
		 * \return Returns address of ptr under which the TFInterpreter has registered this TF
		 */
		virtual TransceiverDeviceInterface::shared_ptr *getTFInterpreterRegistry();

	private:
		/*!
		 * \brief Function to execute
		 */
		TransceiverDeviceInterface::shared_ptr _function;
};

using TransceiverDeviceInterfaceSharedPtr = TransceiverDeviceInterface::shared_ptr;
using TransceiverDeviceInterfaceConstSharedPtr = TransceiverDeviceInterface::const_shared_ptr;

#endif
