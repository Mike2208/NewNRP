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

/*! \page transceiver_function "Transceiver Function"
TransceiverFunctions are user-defined python scripts that facilitate the exchange of data between Engines using Devices.
Every TransceiverFunction must be defined via a TransceiverFunctionConfig in the SimulationConfig, as well as a python file. The config must reference the file.
The file must contain a function with a TransceiverFunction decorator.

Two additional classes are used to manage TransceiverFunctions: The TransceiverFunctionManager and the TransceiverFunctionInterpreter. The TransceiverFunctionManager
handles config file reading/writing. The TransceiverFunctionInterpreter handles execution of the actual python functions. Additionally, it contains a buffer to store Device
data. This buffer can be accessed and manipulated by the TransceiverFunctions, and can be sent to/received from engines.

On SimulationLoop startup, all TransceiverFunctionConfig are read. For each config, the python file that contains the TransceiverFunction is determined and executed.
During execution, the TF will register itself to the TransceiverFunctionInterpreter using TransceiverDeviceInterface::getTFInterpreterRegistry.

Users must be able to access device data from engines. This is facilitated by Python decorators that can be prepended to the TF.
All decorators are listed \ref tf_decorators "here".

Additionaly, TFs allow users to send data to engines as well. This is accomplished via the return value of the TF. Should a user wish to send device data to an engine, the TF
must return a list of devices. This list will be parsed and sent to the appropriate engine once all TFs have been executed.

An example of a TF is listed below. Using the SingleTransceiverDevice decorator, it reads Device data of type py_obj from an Engine with name python_1. During execution, the
TF takes the data from the device and places it into rec_device1. rec_device1 is then sent to a separate engine called python_2 by using the return value of the TF. Also, as
can be seen by the TransceiverFunction decorator, this TF is linked to engine python_2, meaning it will be executed before the start of every python_2 engine iteration.

\code{.py}
from NRPPythonModule import *

@SingleTransceiverDevice(keyword='device1', id=DeviceIdentifier('device1', 'python_1', 'py_obj'))
@TransceiverFunction("python_2")
def transceiver_function(device1):
	rec_device1 = PythonDevice(device1.id, device1.data)

	rec_device1.name = "rec_device1"
	rec_device1.engine_name = "python_2"

	return [ rec_device1 ]
\endcode
 */

#endif // TRANSCEIVER_FUNCTION_H
