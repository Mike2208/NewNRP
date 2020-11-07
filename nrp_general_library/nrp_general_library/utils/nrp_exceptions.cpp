#include "nrp_general_library/utils/nrp_exceptions.h"


NRPException::~NRPException() = default;

const char *NRPException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW
{
	return this->_errMsg.data();
}

template<>
void NRPException::logCreate<void>(const std::string &msg, spdlog_out_fcn_t spdlog_call)
{
	std::invoke(spdlog_call, msg);
}

NRPExceptionNonRecoverable::~NRPExceptionNonRecoverable() = default;

NRPExceptionRecoverable::~NRPExceptionRecoverable() = default;
