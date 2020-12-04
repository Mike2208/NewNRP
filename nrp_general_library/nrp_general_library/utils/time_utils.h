#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <chrono>

using SimulationTime = std::chrono::microseconds;

/*!
 * \brief Converts given value to SimulationTime object
 * 
 * The function is parametrized with two parameters: vartype and ratio.
 * Vartype is the type of variable that is supposed to be converted to SimulationTime.
 * Usually this will be int or float. Ratio is std::ratio class.
 */
template <class vartype, class ratio>
static SimulationTime toSimulationTime(vartype time)
{
	return std::chrono::duration_cast<SimulationTime>(std::chrono::duration<vartype, ratio>(time));
}

/*!
 * \brief Converts SimulationTime object to specified type and with given ratio
 */
template <class vartype, class ratio>
static vartype fromSimulationTime(SimulationTime time)
{
	return std::chrono::duration_cast<std::chrono::duration<vartype, ratio>>(time).count();
}

#endif // TIME_UTILS_H

// EOF
