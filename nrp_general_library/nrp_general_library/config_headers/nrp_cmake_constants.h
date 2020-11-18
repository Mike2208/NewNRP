#ifndef NRP_CMAKE_CONSTANTS_H
#define NRP_CMAKE_CONSTANTS_H

/* Automatically created by cmake configure file from nrp_general_library/config_headers/nrp_cmake_constants.h.in
*  DO NOT EDIT DIRECTLY!
*/

/*!
 * \brief Name of python module library
 */
#define PYTHON_MODULE_NAME NRPPythonModule

/*!
 * \brief Name of python module library as string
 */
#define PYTHON_MODULE_NAME_STR "NRPPythonModule"

/*!
 * \brief Install Location
 */
#define NRP_INSTALL_DIR "/usr/local/nrp_grpc"

/*!
 *	\brief Library Install location
 */
#define NRP_LIB_INSTALL_DIR "/usr/local/nrp_grpc/lib"

/*!
 *	\brief NRP Plugin install directory
 */
#define NRP_PLUGIN_INSTALL_DIR ""

/*!
 *	\brief Bash cmd to set environment before starting engine
 */
#define NRP_ENGINE_SET_ENV_CMD "/usr/local/nrp_grpc/sbin/set_env_exec.sh"

/*!
 * \brief Parameter to pass to NRP_ENGINE_SET_ENV_CMD if child env variables should be sanitized before executing.
 * This will clear the entire environment, so essential variables such as PATH, LD_LIBRARY_PATH, PYTHON_PATH must be set manually
 */
#define NRP_CLEAR_ENV "NRP_CLEAR_ENV"

#endif // NRP_CMAKE_CONSTANTS_H
