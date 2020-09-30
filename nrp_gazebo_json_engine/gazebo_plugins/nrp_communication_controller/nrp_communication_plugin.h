#ifndef NRP_COMMUNICATION_PLUGIN_H
#define NRP_COMMUNICATION_PLUGIN_H

#include <gazebo/gazebo.hh>
#include <string_view>

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
			 * \brief Server URL, read from program opts
			 */
			std::string _serverAddress;
	};

	GZ_REGISTER_SYSTEM_PLUGIN(NRPCommunicationPlugin)
}

#endif
