//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_general_library/utils/spdlog_setup.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

#include <iomanip>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <unistd.h>
#include <vector>

void SPDLogSetup::shutdownDefault()
{
	spdlog::shutdown();
}

SPDLogSetup::SPDLogSetup(std::filesystem::path &&baseFilename, FILE *consoleOut, spdlog::level::level_enum logLevel)
{
	// Append current date and time to filename
	try
	{
		const auto t = std::time(nullptr);
		const auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "-%Y-%m-%d-%H:%M:%S-") << getpid() << ".log";

		baseFilename += oss.str();
	}
	catch(std::exception &e)
	{
		const auto errMsg = std::string("Failed to get current time: ") + e.what();
		fprintf(consoleOut, "%s\n", errMsg.data());
		throw std::runtime_error(errMsg);
	}

	// Create sinks for filename and console
	std::vector<spdlog::sink_ptr> sinks;
	try
	{
		sinks.emplace_back(new spdlog::sinks::basic_file_sink_st(baseFilename));
		sinks.emplace_back(new spdlog::sinks::ansicolor_sink<spdlog::details::console_nullmutex>(consoleOut, spdlog::color_mode::automatic));
	}
	catch(std::exception &e)
	{
		const auto errMsg = std::string("Failed to create spdlogger sinks: ") + e.what();
		fprintf(consoleOut, "%s\n", errMsg.data());
		throw std::runtime_error(errMsg);
	}

	// Create logger
	try
	{
		std::shared_ptr<spdlog::logger> nrpLoggerPtr(new spdlog::logger(SPDLogSetup::LoggerName.data(), sinks.begin(), sinks.end()));
		nrpLoggerPtr->set_level(logLevel);

		// Set new logger as default
		spdlog::register_logger(nrpLoggerPtr);
		spdlog::set_default_logger(nrpLoggerPtr);
	}
	catch(std::exception &e)
	{
		const auto errMsg = std::string("Failed to create and register spdlogger: ") + e.what();
		fprintf(consoleOut, "%s\n", errMsg.data());
		throw std::runtime_error(errMsg);
	}
}

SPDLogSetup::~SPDLogSetup()
{
	SPDLogSetup::shutdownDefault();
}

spdlog::logger &SPDLogSetup::nrpLogger() const
{
	return *(spdlog::get(SPDLogSetup::LoggerName.data()));
}

