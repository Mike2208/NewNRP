#ifndef NRP_NEST_CMAKE_CONSTANTS_H
#define NRP_NEST_CMAKE_CONSTANTS_H

/*!
 * \file nrp_nest_constants.h
 * \brief Contains constants exported from cmake. Used to set parameters dependent upon the build and installation process.
 * DO NOT EDIT THIS HEADER FILE! The code is automatically generated via cmake from nrp_nest_server_engine/config/nrp_nest_cmake_constants.h.in
 */

/*!
 * \brief Path to PyNest python package
 */
#define NRP_PYNEST_PATH "/lib/python3.6/site-packages"

/*!
 * \brief Path to NRP Nest Executable
 */
#define NRP_NEST_BIN_PATH "/bin"

/*!
 * \brief Name of python module
 */
#define NRP_NEST_PYTHON_MODULE NRPNestServerPythonModule

/*!
 * \brief Name of python module (as string)
 */
#define NRP_NEST_PYTHON_MODULE_STR "NRPNestServerPythonModule"

/*!
 * \brief Name of NRP Nest Executable
 */
#define NRP_NEST_SERVER_EXECUTABLE_PATH "/usr/local/nrp/bin/nest-server"

#endif // NRP_NEST_CMAKE_CONSTANTS_H
