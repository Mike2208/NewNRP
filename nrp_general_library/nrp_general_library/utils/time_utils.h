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

#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <chrono>

using SimulationTime = std::chrono::nanoseconds;

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
