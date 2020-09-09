#ifndef PYTHON_INTERPRETER_STATE_H
#define PYTHON_INTERPRETER_STATE_H

#include "nrp_general_library/utils/wchar_t_converter.h"

#include <boost/python.hpp>
#include <stack>

/*!
 * \brief Initializes the python interpreter as well as python threading
 */
class PythonInterpreterState
{
	public:
		/*!
		 * \brief Constructor. Initializes Python with the given start parameters, enables threading, and releases GIL
		 * \param argc main()'s argc
		 * \param argv main()'s argv
		 */
		PythonInterpreterState(int argc, char *argv[], bool allowThreads = false);

		/*!
		 * \brief Allow execution of other threads. If this is set, main thread may not execute python code
		 */
		void allowThreads();

		/*!
		 * \brief Are threads currently allowed?
		 * \return Returns true if allowed, false otherwise
		 */
		bool threadsAllowed() const;

		/*!
		 * \brief Halt other threads from executin. This is required if python code should be executed in the main thread
		 */
		void endAllowThreads();

		/*!
		 *	\brief Destructor. Reestablishes thread state
		 */
		~PythonInterpreterState();

	private:
		/*!
		 * \brief Converts argv to wchar_t. Used by Python
		 */
		WCharTConverter _wcharArgs;

		/*!
		 * \brief Initial PyThread state
		 */
		PyThreadState *_state = nullptr;
};

/*!
 * \brief Manages the Pyton GIL. Useful for threads
 */
class PythonGILLock
{
	public:
		/*!
		 * \brief Constructor. Acquires GIL if requested
		 * \param state GIL State
		 * \param acquire Should GIL be acquired
		 */
		PythonGILLock(PyGILState_STATE &state, const bool acquire = true);

		/*!
		 *	\brief Releases GIL if previously acquired
		 */
		~PythonGILLock();

		// Disable copy mechanism
		PythonGILLock(const PythonGILLock&)            = delete;
		PythonGILLock &operator=(const PythonGILLock&) = delete;

		/*!
		 * \brief Acquire GIL
		 */
		void acquire();

		/*!
		 * \brief Does this thread have the GIL?
		 */
		static bool hasGIL();

		/*!
		 * \brief Release GIL
		 */
		void release();

	private:
		/*!
		 * \brief Is GIL state acuired?
		 */
		PyGILState_STATE *_state;
};

#endif // PYTHON_INTERPRETER_STATE_H
