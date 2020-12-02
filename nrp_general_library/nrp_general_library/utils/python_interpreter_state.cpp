#include "nrp_general_library/utils/python_interpreter_state.h"


PythonInterpreterState::PythonInterpreterState(int argc, const char *const *argv, bool allowThreads)
	: _wcharArgs(argc, argv)
{
	assert(argc >= 1);

	Py_SetProgramName(this->_wcharArgs.getWCharTPointers()[0]);

	Py_Initialize();
	PyEval_InitThreads();

	PySys_SetArgv(this->_wcharArgs.getWCharSize(), this->_wcharArgs.getWCharTPointers());

	if(allowThreads)
		this->_state = PyEval_SaveThread();
	else
		this->_state = nullptr;
}

PythonInterpreterState::PythonInterpreterState(int argc, const std::vector<const char*> &argv, bool allowThreads)
    : PythonInterpreterState(argc, argv.data(), allowThreads)
{}

PythonInterpreterState::PythonInterpreterState(bool allowThreads)
    : _wcharArgs(0, nullptr)
{
	Py_Initialize();
	PyEval_InitThreads();

	if(allowThreads)
		this->_state = PyEval_SaveThread();
	else
		this->_state = nullptr;
}

void PythonInterpreterState::allowThreads()
{
	if(this->_state == nullptr)
		this->_state = PyEval_SaveThread();
}

bool PythonInterpreterState::threadsAllowed() const
{
	return (this->_state != nullptr);
}

void PythonInterpreterState::endAllowThreads()
{
	if(this->_state != nullptr)
	{
		PyEval_RestoreThread(this->_state);
		this->_state = nullptr;
	}
}

PythonInterpreterState::~PythonInterpreterState()
{
	this->endAllowThreads();
}

PythonGILLock::PythonGILLock(PyGILState_STATE &state, const bool acquire)
	: _state(&state)
{
	if(acquire)
		this->acquire();
}

PythonGILLock::~PythonGILLock()
{
	this->release();
	this->_state = nullptr;
}

void PythonGILLock::acquire()
{
	*(this->_state) = PyGILState_Ensure();
}

void PythonGILLock::release()
{
	PyGILState_Release(*(this->_state));
}
