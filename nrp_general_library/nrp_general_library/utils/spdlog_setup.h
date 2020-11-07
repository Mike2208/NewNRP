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
