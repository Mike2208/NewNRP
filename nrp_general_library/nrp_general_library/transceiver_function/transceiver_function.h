#ifndef TRANSCEIVER_FUNCTION_H
#define TRANSCEIVER_FUNCTION_H

#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"

#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"
#include "nrp_general_library/config/transceiver_function_config.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"

#include <string>
#include <boost/python.hpp>

/*!
 * \brief Holds a single transfer function decorator
 */
class TransceiverFunction
        : public TransceiverDeviceInterface,
          public PtrTemplates<TransceiverFunction>
{
	public:
		/*!
		 * \brief Constructor
		 * \param linkedEngine Name of linked engine.
		 * This TF will only be called whenever Said engine finishes a timestep
		 */
		TransceiverFunction(std::string linkedEngine);
		virtual ~TransceiverFunction() override = default;

		const std::string &linkedEngineName() const override;

		/*!
		 * \brief Decorator __call__() function. Takes the Transfer Function as a parameter. Moves this class into a shared_ptr.
		 * \param transceiverFunction User defined TF
		 * \return shared_ptr referencing data from this object
		 */
		TransceiverDeviceInterface::shared_ptr pySetup(boost::python::object transceiverFunction);

		/*!
		 * \brief Execute the transfer function
		 * \param args Python args
		 * \param kwargs Python keywords
		 * \return Returns result of TF
		 */
		boost::python::object runTf(boost::python::tuple &args, boost::python::dict &kwargs) override;

	protected:
		EngineInterface::device_identifiers_t getRequestedDeviceIDs() const override;

		EngineInterface::device_identifiers_t updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs) const override;

	private:
		/*!
		 * \brief Transfer function that should be executed
		 */
		boost::python::object _function;

		/*!
		 * \brief Name of linked Engine
		 */
		std::string _linkedEngine;

		/*!
		 * \brief Pointer to location where TFInterperter has stored this TF
		 */
		TransceiverDeviceInterface::shared_ptr *_tfInterpreterRegistryPtr = nullptr;

		/*!
		 * \brief Gets pointer to location where this TF is stored
		 * \return Returns _tfInterpreterRegistryPtr
		 */
		TransceiverDeviceInterface::shared_ptr *getTFInterpreterRegistry() override final;
};

#endif // TRANSCEIVER_FUNCTION_H
