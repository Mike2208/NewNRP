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

#ifndef NRP_COMMUNICATION_PLUGIN_H
#define NRP_COMMUNICATION_PLUGIN_H

#include <gazebo/gazebo.hh>
#include <string_view>

#include "nrp_general_library/utils/python_interpreter_state.h"

namespace gazebo
{
	/*!
	 * \brief Plugin that creates a REST server for NRP communication
	 */
	class NRPCommunicationPlugin
	        : public SystemPlugin
	{
		public:
			virtual ~NRPCommunicationPlugin() = default;

			/*!
			 * \brief Load the communication server
			 */
			virtual void Load(int argc = 0, char **argv = nullptr);

			virtual void Reset();

		private:
			/*!
			 * \brief Python State. Used for python interpreter
			 */
			PythonInterpreterState _pyState;

			/*!
			 * \brief Communication thread
			 */
			std::future<std::string> _commThread;

			/*!
			 * \brief MPI Communication function. Will continuously wait for new MPI commands
			 * \return Returns empty string on success, error message on error
			 */
			static std::string commThreadFcn() noexcept;
	};

	GZ_REGISTER_SYSTEM_PLUGIN(NRPCommunicationPlugin)
}

#endif
