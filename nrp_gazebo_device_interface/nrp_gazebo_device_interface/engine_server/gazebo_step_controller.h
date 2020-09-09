#ifndef GAZEBO_STEP_CONTROLLER_H
#define GAZEBO_STEP_CONTROLLER_H

/*!
 * \brief Controlls execution of Gazebo steps. Will be inherited by a Gazebo WorldPlugin
 */
class GazeboStepController
{
	public:
		GazeboStepController();
		virtual ~GazeboStepController();

		virtual double runLoopStep(double timeStep) = 0;

		virtual bool finishWorldLoading() = 0;
};

#endif // GAZEBO_STEP_CONTROLLER_H
