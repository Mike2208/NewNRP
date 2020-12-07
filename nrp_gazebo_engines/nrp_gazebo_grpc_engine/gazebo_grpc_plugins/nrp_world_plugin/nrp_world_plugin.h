#ifndef NRP_WORLD_PLUGIN_H
#define NRP_WORLD_PLUGIN_H

#include "nrp_gazebo_grpc_engine/engine_server/gazebo_step_controller.h"

#include <gazebo/gazebo.hh>
#include <gazebo/physics/JointController.hh>
#include <gazebo/physics/Joint.hh>

namespace gazebo
{
	/*!
	 * \brief Interface for a single joint
	 */
	class NRPWorldPlugin
	        : public GazeboStepController,
	          public WorldPlugin
	{
		public:
			virtual ~NRPWorldPlugin() override = default;

			virtual void Load(physics::WorldPtr world, sdf::ElementPtr sdf) override;
			virtual void Reset() override;

			virtual SimulationTime runLoopStep(SimulationTime timeStep) override;

			bool finishWorldLoading() override;

		private:
			/*!
			 * \brief Lock to ensure only one loop is being executed
			 */
			std::mutex _lockLoop;

			physics::WorldPtr _world;
			sdf::ElementPtr _worldSDF;

			/*!
			 * \brief Start running the sim.
			 * \param numIterations Number of iterations to run
			 */
			void startLoop(unsigned int numIterations);
	};

	GZ_REGISTER_WORLD_PLUGIN(NRPWorldPlugin)
}

#endif // NRP_WORLD_PLUGIN
