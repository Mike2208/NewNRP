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

#ifndef NRP_EXCEPTIONS_H
#define NRP_EXCEPTIONS_H

#include "nrp_general_library/utils/nrp_logger.h"

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
		/*!
		 * \brief Extract NRPException from EXCEPTION if possible
		 * \tparam EXCEPTION Exception type
		 * \param exception Exception data
		 * \return Returns ptr to casted exception, nullptr if not possible
		 */
		template<class EXCEPTION>
		static NRPException *nrpException(EXCEPTION &exception) noexcept
		{
			try
			{
				return dynamic_cast<NRPException*>(&exception);
			}
			catch(std::exception&)
			{
				return nullptr;
			}
		}

	public:
		template<class EXCEPTION>
		static void logOnce(EXCEPTION &exception, NRPLogger::spdlog_out_fcn_t spdlogCall = NRPLogger::SPDErrLogDefault)
		{
			NRPException *const logData = NRPException::nrpException(exception);
			if(logData == nullptr)
				std::invoke(spdlogCall, exception.what());
			else if(logData->_msgLogged != true)
			{
				std::invoke(spdlogCall, exception.what());
				logData->_msgLogged = true;
			}
		}

		template<class EXCEPTION = NRPExceptionNonRecoverable, class LOG_EXCEPTION_T>
		requires(std::constructible_from<EXCEPTION, const std::string&>)
		static EXCEPTION logCreate(LOG_EXCEPTION_T &exception, const std::string &msg, NRPLogger::spdlog_out_fcn_t spdlogCall = NRPLogger::SPDErrLogDefault)
		{
			NRPException::logOnce(exception, spdlogCall);

			std::invoke(spdlogCall, msg);
			if constexpr (std::constructible_from<EXCEPTION, const std::string&, bool>)
			{	return EXCEPTION(msg, true);	}
			else
			{	return EXCEPTION(msg);	}
		}

		/*!
		 * \brief Logs the given message to the output, then returns EXCEPTION type
		 * \tparam EXCEPTION Exception type to return
		 * \param msg Message to log and put into thrown exception
		 * \param spdlogCall spdlog function to call for logging
		 */
		template<class EXCEPTION = NRPExceptionNonRecoverable>
		requires(std::constructible_from<EXCEPTION, const std::string&> || std::same_as<EXCEPTION, void>)
		static EXCEPTION logCreate(const std::string &msg, NRPLogger::spdlog_out_fcn_t spdlogCall = NRPLogger::SPDErrLogDefault)
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
void NRPException::logCreate<void>(const std::string &msg, NRPLogger::spdlog_out_fcn_t spdlogCall);


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
