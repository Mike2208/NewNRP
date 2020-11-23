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
