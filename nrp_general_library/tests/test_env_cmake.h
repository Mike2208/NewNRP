#ifndef TEST_ENV_CMAKE_H
#define TEST_ENV_CMAKE_H

#include "nrp_general_library/device_interface/device_interface.h"
#include "tests/test_mpi_communication_devices.h"

#define TEST_TRANSCEIVER_FCN_FILE_NAME "/home/majster/NewNRP/nrp_general_library/tests/test_files/transceiver_function.py"
#define TEST_INVALID_TRANSCEIVER_FCN_FILE_NAME "/home/majster/NewNRP/nrp_general_library/tests/test_files/invalid_transceiver_function.py"

#define TEST_PLUGIN_DIR "/home/majster/NewNRP/build/nrp_general_library"
#define TEST_NRP_PLUGIN "TestNRPPluginManager.so"

#define TEST_PYTHON_MODULE_NAME NRPGeneralPythonTestModule
#define TEST_PYTHON_MODULE_NAME_STR "NRPGeneralPythonTestModule"

#define TEST_NRP_PROCESS_EXEC "/home/majster/NewNRP/build/nrp_general_library/TestNRPProcessExec"

#define TEST_PROC_STR_START   "start"
#define TEST_PROC_STR_SIGTERM "sigterm"
#define TEST_PROC_STR_FAIL    "fails"

#define TEST_PROC_ENV_VAR_NAME "TEST_VAR"
#define TEST_PROC_ENV_VAR_VAL "test_var"

#define TEST_MPI_COMMUNICATION_PROC "/home/majster/NewNRP/build/nrp_general_library/TestMPIComm"
#define TEST_MPI_COMM_STR "teststr"
#define TEST_MPI_COMM_DEVID DeviceIdentifier("name", "type", "engine")
#define TEST_MPI_COMM_DEV1 TestDevice1(3, "ret", boost::python::dict())

#endif // TEST_ENV_CMAKE_H
