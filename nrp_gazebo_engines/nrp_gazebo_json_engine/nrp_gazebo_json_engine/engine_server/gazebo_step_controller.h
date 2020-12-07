#ifndef GAZEBO_STEP_CONTROLLER_H
#define GAZEBO_STEP_CONTROLLER_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"

/*!
 *  \brief Controlls execution of Gazebo steps. Will be inherited by a Gazebo WorldPlugin
 */
class GazeboStepController
{
	public:
		GazeboStepController();
		virtual ~GazeboStepController();

		virtual SimulationTime runLoopStep(SimulationTime timeStep) = 0;

		virtual bool finishWorldLoading() = 0;
};

/*! \addtogroup gazebo_json_engine
 *  The loop is controlled via GazeboStepController.
 */

#endif // GAZEBO_STEP_CONTROLLER_H
