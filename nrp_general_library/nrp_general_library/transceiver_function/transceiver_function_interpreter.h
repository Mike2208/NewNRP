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

#ifndef TRANSCEIVER_FUNCTION_INTERPRETER_H
#define TRANSCEIVER_FUNCTION_INTERPRETER_H

#include "nrp_general_library/config/transceiver_function_config.h"
#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"

#include <vector>
#include <map>
#include <memory>
#include <list>
#include <boost/python.hpp>

/*!
 * \brief Python Interpreter to manage transfer function calls
 */
class TransceiverFunctionInterpreter
{
		/*!
		 * \brief Data associated with one TF
		 */
		struct TransceiverFunctionData
		{
			/*!
			 * \brief Name of Transfer Function
			 */
			std::string Name;

			/*!
			 * \brief Pointer to TransceiverFunction
			 */
			TransceiverDeviceInterface::shared_ptr TransceiverFunction = nullptr;

			/*!
			 * \brief Devices requested by TF
			 */
			EngineInterface::device_identifiers_t DeviceIDs;

			/*!
			 * \brief Local variables used by this TransceiverFunction
			 */
			boost::python::object LocalVariables;

			TransceiverFunctionData() = default;
			TransceiverFunctionData(const std::string &_name, const TransceiverDeviceInterface::shared_ptr &_transceiverFunction, const EngineInterface::device_identifiers_t &_deviceIDs, const boost::python::object &_localVariables);
		};

		using local_dict_objects_t = std::map<std::string, boost::python::dict>;
		using transceiver_function_datas_t = std::map<std::string, TransceiverFunctionData>;

	public:
		using device_list_t = boost::python::list;
		using engines_devices_t = std::map<std::string, const EngineInterface::device_outputs_t*>;

		/*!
		 * \brief Result of a single TF run
		 */
		struct TFExecutionResult
		{
			/*!
			 * \brief Python Device List
			 */
			device_list_t DeviceList;

			/*!
			 * \brief Extracted Pointers to devices in DeviceList
			 */
			std::vector<DeviceInterface*> Devices;

			/*!
			 * \brief Extract devices from DeviceList and insert them into Devices
			 */
			void extractDevices();

			TFExecutionResult(device_list_t &&_deviceList);
		};

		TransceiverFunctionInterpreter();
		TransceiverFunctionInterpreter(const boost::python::dict &tfGlobals);

		/*!
		 * \brief Find TF with given name
		 * \param name Name to find
		 * \return Returns iterator to TF. If name not present, returns _transceiverFunctions.end()
		 */
		transceiver_function_datas_t::const_iterator findTF(const std::string &name) const;

		/*!
		 * \brief Reference to loaded TFs
		 * \return Returns reference to loaded TFs
		 */
		const transceiver_function_datas_t &loadedTFs() const;

		/*!
		 * \brief Get Device IDs requested by TFs
		 * \return
		 */
		EngineInterface::device_identifiers_t updateRequestedDeviceIDs() const;

		/*!
		 * \brief Set EngineInterface pointers. Used by TransceiverFunctions to access devices
		 * \param engines Mapping from engine name to engine ptr
		 */
		void setEngineDevices(engines_devices_t &&engineDevices);

		/*!
		 * \brief Access engine map
		 */
		constexpr const engines_devices_t &engineDevices() const
		{	return this->_engineDevices;	}

		/*!
		 * \brief Execute one transfer function.
		 * \param tfName Name of function to execute
		 * \return Returns result of execution. Contains a list of device commands
		 */
		boost::python::object runSingleTransceiverFunction(const std::string &tfName);

		/*!
		 * \brief Execute one transfer function
		 * \param tfData TransceiverFunction to execute
		 * \return Returns result of execution. Contains a list of device commands
		 */
		boost::python::object runSingleTransceiverFunction(const TransceiverFunctionData &tfData);

		/*!
		 * \brief Get TFs linked to specific engine
		 * \param engineName Name of engine
		 * \return Returns range of TFs linked to engine name
		 */
		std::pair<transceiver_function_datas_t::iterator, transceiver_function_datas_t::iterator> getLinkedTFs(const std::string &engineName);

		/*!
		 * \brief Prepares a TF for execution. Loads code into storage
		 * \param transceiverFunction Pointer to TF configuration
		 * \return Returns iterator to loaded TF
		 */
		transceiver_function_datas_t::iterator loadTransceiverFunction(const TransceiverFunctionConfig &transceiverFunction);

		/*!
		 * \brief Prepares a TF for execution. Loads code into storage
		 * \param tfName Name of this TF
		 * \param transceiverFunction Ptr to the TF
		 * \param localVars Local Python variables required by this TF
		 * \return Returns iterator to loaded TF
		 */
		transceiver_function_datas_t::iterator loadTransceiverFunction(const std::string &tfName, const TransceiverDeviceInterfaceSharedPtr &transceiverFunction, boost::python::object &&localVars = boost::python::object());

		/*!
		 * \brief Updates TF with the given name
		 * \param name Name of TF
		 * \param transceiverFunction New TF configuration
		 * \return Returns iterator to updated TF
		 */
		transceiver_function_datas_t::iterator updateTransceiverFunction(const TransceiverFunctionConfig &transceiverFunction);

	protected:
		/*!
		 * \brief Registers a new transfer function. Used by TransceiverFunction to automatically register itself with the interpreter upon creation
		 * \param linkedEngine Name of linked engine
		 * \param transceiverFunction Transfer Function to register
		 * \return Returns pointer to stored location. Used by TransceiverDeviceInterfaceGeneral to automatically update the registered function when an upper decorator runs pySetup
		 */
		TransceiverDeviceInterfaceSharedPtr *registerNewTF(const std::string &linkedEngine, const TransceiverDeviceInterfaceSharedPtr &transceiverFunction);

	private:
		/*!
		 * \brief Global dictionary
		 */
		boost::python::dict _globalDict;

		/*!
		 * \brief All loaded transceiver functions, mapped to linked engines
		 */
		transceiver_function_datas_t _transceiverFunctions;

		/*!
		 * \brief Engine Map. From engine name to devices
		 */
		engines_devices_t _engineDevices;

		/*!
		 * \brief Pointer to newly created TransceiverFunction
		 */
		transceiver_function_datas_t::iterator _newTFIt = this->_transceiverFunctions.end();

		// Give TransceiverFunction access to TransceiverFunctionInterpreter::registerNewTF()
		friend class TransceiverFunction;
};

using TransceiverFunctionInterpreterSharedPtr = std::shared_ptr<TransceiverFunctionInterpreter>;
using TransceiverFunctionInterpreterConstSharedPtr = std::shared_ptr<const TransceiverFunctionInterpreter>;

#endif // TRANSCEIVER_FUNCTION_INTERPRETER_H
