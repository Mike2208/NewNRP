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

#ifndef NRP_LOGGER_H
#define NRP_LOGGER_H

#include <spdlog/spdlog.h>

/*!
 * \brief NRP Logging functions
 */
class NRPLogger
{
	public:
		/*!
		 * \brief Logging function type
		 */
		using spdlog_out_fcn_t = void(&)(const std::string&);

		/*!
		 * \brief Error logging function
		 */
		static constexpr spdlog_out_fcn_t SPDErrLogDefault = spdlog::error<std::string>;

		/*!
		 * \brief Warning logging function
		 */
		static constexpr spdlog_out_fcn_t SPDWarnLogDefault = spdlog::warn<std::string>;

		/*!
		 * \brief Info logging function
		 */
		static constexpr spdlog_out_fcn_t SPDInfoLogDefault = spdlog::info<std::string>;

		/*!
		 * \brief Debug logging function
		 */
		static constexpr spdlog_out_fcn_t SPDDebugLogDefault = spdlog::debug<std::string>;
};

#endif // NRP_LOGGER_H
