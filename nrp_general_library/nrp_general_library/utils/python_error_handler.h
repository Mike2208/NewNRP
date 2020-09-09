#ifndef PYTHON_ERROR_HANDLER_R
#define PYTHON_ERROR_HANDLER_R

#include <string>

/*!
 * \brief Read out a properly formatted Python exception string. Only call if a Python exception was thrown
 * \return Returns human-readable error string
 */
extern std::string handle_pyerror();

#endif // PYTHON_ERROR_HANDLER_R
