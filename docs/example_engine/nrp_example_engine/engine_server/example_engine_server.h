#ifndef EXAMPLE_ENGINE_SERVER_H
#define EXAMPLE_ENGINE_SERVER_H

#include "nrp_example_engine/config/example_config.h"
#include "nrp_example_engine/devices/example_device.h"
#include "nrp_example_engine/engine_server/example_engine_device_controller.h"

class ExampleEngineServer
{
	public:
		ExampleEngineServer(const std::string &engineName);
		~ExampleEngineServer() = default;

		bool initRunFlag() const;
		bool shutdownFlag() const;

		constexpr const std::string &engineName() const
		{	return this->_engineName;	}

		float runLoopStep(float timeStep);
		void initialize();
		void shutdown();

	private:
		bool _initRunFlag = false;
		bool _shutdownFlag = false;

		std::string _engineName;
};

#endif // EXAMPLE_ENGINE_SERVER_H
