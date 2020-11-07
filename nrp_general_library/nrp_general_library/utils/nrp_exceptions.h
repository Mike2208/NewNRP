#ifndef NRP_EXCEPTIONS_H
#define NRP_EXCEPTIONS_H

#include <concepts>
#include <exception>
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>

class NRPExceptionNonRecoverable;

/*!
 * \brief Base NRPException class
 */
class NRPException
        : public std::exception
{
	public:
		using spdlog_out_fcn_t = void(&)(const std::string&);
		static constexpr spdlog_out_fcn_t SPDErrLogDefault = spdlog::error<std::string>;

		/*!
		 * \brief Logs the given message to the output, then returns EXCEPTION type
		 * \tparam EXCEPTION Exception type to return
		 * \param msg Message to log and put into thrown exception
		 * \param spdlogCall spdlog function to call for logging
		 */
		template<class EXCEPTION = NRPExceptionNonRecoverable>
		requires(std::constructible_from<EXCEPTION, const std::string&> || std::same_as<EXCEPTION, void>)
		static EXCEPTION logCreate(const std::string &msg, spdlog_out_fcn_t spdlogCall = SPDErrLogDefault)
		{
			std::invoke(spdlogCall, msg);
			if constexpr (std::constructible_from<EXCEPTION, const std::string&, bool>)
			{	return EXCEPTION(msg, true);	}
			else
			{	return EXCEPTION(msg);	}
		}

		template<class T>
		requires(std::constructible_from<std::string, T>)
		explicit NRPException(T &&msg, bool msgLogged = false)
		    : _errMsg(std::forward<T>(msg)),
		      _msgLogged(msgLogged)
		{}

		~NRPException() override;

		const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

	protected:
		std::string _errMsg = "";

	private:
		bool _msgLogged = false;
};

template<>
void NRPException::logCreate<void>(const std::string &msg, spdlog_out_fcn_t spdlog_call);

/*!
 * \brief Exception for non-recoverable errors
 */
class NRPExceptionNonRecoverable
        : public NRPException
{
	public:
		template<class T>
		explicit NRPExceptionNonRecoverable(T &&msg, bool msgLogged = false)
		    : NRPException(std::forward<T>(msg), msgLogged)
		{}
		~NRPExceptionNonRecoverable() override;
};

/*!
 * \brief Exception for recoverable errors
 */
class NRPExceptionRecoverable
        : public NRPException
{
	public:
		template<class T>
		explicit NRPExceptionRecoverable(T &&msg, bool msgLogged = false)
		    : NRPException(std::forward<T>(msg), msgLogged)
		{}
		~NRPExceptionRecoverable() override;
};


#endif // NRP_EXCEPTIONS_H
