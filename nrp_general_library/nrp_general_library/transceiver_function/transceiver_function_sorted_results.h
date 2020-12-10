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

#ifndef TRANSCEIVER_FUNCTION_SORTED_RESULTS_H
#define TRANSCEIVER_FUNCTION_SORTED_RESULTS_H

#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"
#include "nrp_general_library/transceiver_function/transceiver_function_manager.h"

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"

#include <map>

struct TransceiverFunctionSortedResults
        : public std::map<std::string, EngineInterface::device_inputs_t>
{
	using devices_t = EngineInterface::device_inputs_t;
	using interface_results_t = std::map<std::string, devices_t>;

	/*!
	 * \brief Add additional results to existing SortedResults
	 * \param results Results to add
	 */
	void addResults(const TransceiverFunctionManager::tf_results_t &results);

	/*!
	 * \brief Sort results according to interface type
	 * \param results Results to sort
	 * \return Returns sorted results
	 */
	static TransceiverFunctionSortedResults sortResults(const TransceiverFunctionManager::tf_results_t &results);

	private:
	    /*!
		 * \brief All python lists. Manage them to prevent devices from destructing before their respective device_inputs_t
		 */
	    std::list<TransceiverFunctionInterpreter::device_list_t> _pyList;

		/*!
		 * \brief Adds TF Result to the map corresponding to the result interface
		 * \param result Device to add
		 * \result Returns iterator to mapping in which device is stored
		 */
		interface_results_t::iterator addResult(DeviceInterface *result);
};

#endif // TRANSCEIVER_FUNCTION_SORTED_RESULTS_H
