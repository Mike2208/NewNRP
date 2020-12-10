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

#ifndef SPDLOG_SETUP_H
#define SPDLOG_SETUP_H

#include <filesystem>
#include <stdio.h>
#include <spdlog/spdlog.h>

/*!
 * \brief Sets up spdlog, creates sinks and sets the logger level
 */
class SPDLogSetup
{
	public:
		/*!
		 * \brief Name of NRP Logger
		 */
		static constexpr std::string_view LoggerName = "nrp_logger";

		/*!
		 * \brief Shutdown default logger
		 */
		static void shutdownDefault();

		/*!
		 * \brief Constructor. Creates a logger with outputs to both the given filename as well as the console
		 * \param baseFilename Base Log Output filename.
		 * The constructor will append the current date, time, and pid to the name. The format is baseFilename + "-%Y-%m-%d-%H:%M:%S-%pid.log"
		 * \param consoleOut Console Output File
		 * \param logLevel Log Level
		 */
		SPDLogSetup(std::filesystem::path &&baseFilename, FILE *consoleOut = stderr, spdlog::level::level_enum logLevel = spdlog::level::info);
		~SPDLogSetup();

		/*!
		 * \brief Get nrpLogger
		 */
		spdlog::logger &nrpLogger() const;

	private:
};

#endif // SPDLOG_SETUP_H
