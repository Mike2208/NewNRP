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

