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


\section transceiver_function_developer_overview Developer Overview

TransceiverFunctions are managed by the TransceiverFunctionManager and the TransceiverFunctionInterpreter. The former deals with general tasks such as configuration loading
and de-/activation, while the latter handles the actual python script loading and execution.

When a new TransceiverFunction script is loaded, the corresponding python file is executed. During execution, a callback must exist to allow TFs to register themselves
with the TransceiverFunctionInterpreter. Currently, any function wishing to register as a TF must execute the TransceiverFunctionInterpreter::registerNewTF of the currently
active TransceiverFunctionInterpreter instance. For TFs to recognize this instance, it must set a pointer to itself at TransceiverDeviceInterface::TFInterpreter. This way,
any newly executed TF can register itself.

To make this process easy for users, we have wrapped the setup inside python decorators. These decorators all follow the same procedure as outlined within Python and mirrored
in C++. First, the constructor of each decorator is called consecutively. This is where the arguments given in brackets are passed on. Then, in reverse order, the
__call__() functions of each decorator are executed. These take as argument the previously executed decorator object, or the function object in the case of the bottom-most
decorator. The __call__() function is mirrored with the pySetup function of the corresponding C++ class. Note that all C++ decorator classes must be made available to python
via the NRPPythonModule library, specified in nrp_general_library/nrp_python_device/python_module.cpp. For an explanation of how this is performed via Boost::Python, take a
look at \ref boost_python "this section".

Every decorator may perform any action within its operations. Nevertheless, they must all follow one instruction: At the end of its __call__() function, it must have moved itself
into a shared ptr object. The ptr to this location must be used to override the TFInterpreterRegistrPtr location. The base TransceiverDeviceInterface class contains a helper
method to perform this task called TransceiverDeviceInterface::pySetup. This process ensures that, when the TransceiverFunctionInterpreter attempts to execute the TF, it will
iterate over all decorators consecutively. Lastly, the __call__() function must return the created shared_ptr object to be used by the next decorator in line.

Once a TF and its decorators have registered themselves as described above, the TransceiverFunctionInterpreter may freely execute it. For this, it calls the runTf function of
the registered TransceiverDeviceInterface. If the process mentioned above has been followed, this will correspond to the runTf method of the outermost decorator. It takes two
arguments, a boost::python::tuple, and a boost::python dict, which correspond to the *args and **kwargs of a python function, respectively. Each decorator is free to
manipulate these arguments as they please, the only requirement is that it must at one point call the runTf function of the proceeding decorator. This ensures that all
decorators are executed, with the final one executing the actual TransceiverFunction.

Currently, all TFs share the same PythonInterpreterState, meaning they share the same global and local python variable pool. Therefore, a function/variable defined in one
TF is accessible in another. In the future, it might make sense to change this behavior.
 */

#endif // TRANSCEIVER_FUNCTION_H
