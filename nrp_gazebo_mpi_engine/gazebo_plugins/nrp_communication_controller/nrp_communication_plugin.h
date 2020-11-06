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
