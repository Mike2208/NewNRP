set(PROJECT_NAME "NRPGeneralLibrary")
set(HEADER_DIRECTORY "nrp_general_library")

set(NAMESPACE_NAME "${PROJECT_NAME}")

set(LIBRARY_NAME "${PROJECT_NAME}")
set(PYTHON_MODULE_NAME "NRPPythonModule")
set(EXECUTABLE_NAME "${PROJECT_NAME}Main")
set(TEST_NAME "${PROJECT_NAME}Tests")

set(LIB_EXPORT_NAME "${LIBRARY_NAME}Targets")
set(LIB_CONFIG_NAME "${LIBRARY_NAME}Config")
set(LIB_VERSION_NAME "${LIB_CONFIG_NAME}Version")

cmake_minimum_required(VERSION 3.16)
project("${PROJECT_NAME}" VERSION ${NRP_VERSION})

include(GNUInstallDirs)

# List library build files
set(LIB_SRC_FILES
	nrp_general_library/config/base_config.cpp
	nrp_general_library/config/config_formats/json_config_properties.cpp
	nrp_general_library/config/config_storage.cpp
	nrp_general_library/config/engine_config.cpp
	nrp_general_library/config/simple_engine_config.cpp
	nrp_general_library/config/simulation_config.cpp
	nrp_general_library/config/transceiver_function_config.cpp
	nrp_general_library/device_interface/device_conversion_mechanism.cpp
	nrp_general_library/device_interface/device_interface.cpp
	nrp_general_library/device_interface/device.cpp
	nrp_general_library/device_interface/device_serializer.cpp
	nrp_general_library/device_interface/device_serializer_methods.cpp
	nrp_general_library/device_interface/devices/pyobject_device.cpp
	nrp_general_library/device_interface/python_device.cpp
	nrp_general_library/engine_interfaces/engine_device_controller.cpp
	nrp_general_library/engine_interfaces/engine_interface.cpp
	nrp_general_library/engine_interfaces/engine_launcher_manager.cpp
	nrp_general_library/plugin_system/plugin.cpp
	nrp_general_library/plugin_system/plugin_manager.cpp
	nrp_general_library/process_launchers/launch_commands/basic_fork.cpp
	nrp_general_library/process_launchers/launch_commands/launch_command.cpp
	nrp_general_library/process_launchers/process_launcher.cpp
	nrp_general_library/process_launchers/process_launcher_basic.cpp
	nrp_general_library/process_launchers/process_launcher_manager.cpp
	nrp_general_library/transceiver_function/single_transceiver_device.cpp
	nrp_general_library/transceiver_function/transceiver_function.cpp
	nrp_general_library/transceiver_function/transceiver_device_interface.cpp
	nrp_general_library/transceiver_function/transceiver_function_interpreter.cpp
	nrp_general_library/transceiver_function/transceiver_function_manager.cpp
	nrp_general_library/transceiver_function/transceiver_function_sorted_results.cpp
	nrp_general_library/utils/concepts.cpp
	nrp_general_library/utils/file_finder.cpp
	nrp_general_library/utils/fixed_string.cpp
	nrp_general_library/utils/nrp_exceptions.cpp
	nrp_general_library/utils/nrp_logger.cpp
	nrp_general_library/utils/pipe_communication.cpp
	nrp_general_library/utils/property_template.cpp
	nrp_general_library/utils/ptr_templates.cpp
	nrp_general_library/utils/python_array_converter.cpp
	nrp_general_library/utils/python_error_handler.cpp
	nrp_general_library/utils/python_interpreter_state.cpp
	nrp_general_library/utils/restclient_setup.cpp
	nrp_general_library/utils/serializers/json_property_serializer.cpp
	nrp_general_library/utils/serializers/property_serializer.cpp
	nrp_general_library/utils/serializers/property_serializer_general.cpp
	nrp_general_library/utils/serializers/python_dict_property_serializer.cpp
	nrp_general_library/utils/spdlog_setup.cpp
	nrp_general_library/utils/wchar_t_converter.cpp
	nrp_general_library/utils/zip_container.cpp
)

# List of python module build files
set(PYTHON_MODULE_SRC_FILES
	nrp_python_device/python_module.cpp
)

# List executable build files
set(EXEC_SRC_FILES
)

# List testing build files
set(TEST_SRC_FILES
	tests/base_config.cpp
	tests/device_interface.cpp
	tests/config_storage.cpp
	tests/engine_launcher_manager.cpp
	tests/json_property_serializer.cpp
	tests/plugin_manager.cpp
	tests/property_template.cpp
	tests/python_dict_property_serializer.cpp
	tests/test_process_launcher_basic.cpp
	tests/transceiver_function_interpreter.cpp
)


##########################################
## Dependencies

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FetchContent)

# cxxopts: Start Parameter Processing
find_package(cxxopts 2.2 QUIET)
if(NOT ${cxxopts_FOUND})
	message("Please wait. Downloading cxxopts...")
	FetchContent_Declare(cxxopts
		GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
		GIT_TAG v2.2.1)

	FetchContent_GetProperties(cxxopts)
	if(NOT cxxopts_POPULATED)
		FetchContent_Populate(cxxopts)

		set(CXXOPTS_BUILD_EXAMPLES OFF)
		set(CXXOPTS_BUILD_TESTS OFF)

		add_subdirectory(${cxxopts_SOURCE_DIR} ${cxxopts_BINARY_DIR})
		add_library(cxxopts::cxxopts ALIAS cxxopts)
	endif()
endif()

# restclient-cpp: REST client library
find_package(restclient-cpp QUIET)
if(NOT ${restclient-cpp_FOUND})
	message("Please wait. Downloading restclient-cpp...")
	FetchContent_Declare(restclient-cpp
		GIT_REPOSITORY https://github.com/mrtazz/restclient-cpp.git
		GIT_TAG c140e62ecc793533ad40794c17d2982121aebc0b)

	FetchContent_GetProperties(restclient-cpp)
	if(NOT restclient-cpp_POPULATED)
		FetchContent_Populate(restclient-cpp)
		add_subdirectory(${restclient-cpp_SOURCE_DIR} ${restclient-cpp_BINARY_DIR})
		target_include_directories(restclient-cpp
			INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
	endif()
endif()

# pistache: REST server library
find_package(Pistache REQUIRED)

# nlohmann_json: JSON library
find_package(nlohmann_json 3.9.0 QUIET)
if(NOT ${nlohmann_json_FOUND})
	message("Please wait. Downloading a newer version of nlohmann_json...")
	FetchContent_Declare(json
		GIT_REPOSITORY https://github.com/nlohmann/json.git
		GIT_TAG v3.9.0)

	FetchContent_GetProperties(json)
	if(NOT json_POPULATED)
		FetchContent_Populate(json)
		add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
	endif()
endif()

# libzip: ZIP Manipulation
find_package(PkgConfig REQUIRED)
pkg_check_modules(LibZip REQUIRED IMPORTED_TARGET libzip)


##########################################
## Header configuration

set(NRP_CLEAR_ENV "NRP_CLEAR_ENV")

# General Header defines
configure_file("nrp_general_library/config/cmake_constants.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/${HEADER_DIRECTORY}/config/cmake_constants.h" @ONLY)

# Python Module defines
configure_file("nrp_python_device/__init__.py.in" "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py" @ONLY)

# Process startup script
configure_file("cmake/set_env_exec.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/set_env_exec.sh" @ONLY)


##########################################
# NRPGeneralLibrary
add_library("${LIBRARY_NAME}" SHARED ${LIB_SRC_FILES})
add_library(${NAMESPACE_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

set(NRP_GEN_LIB_TARGET "${NAMESPACE_NAME}::${LIBRARY_NAME}" CACHE INTERNAL "NRP General Library target")
set(NRP_PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" CACHE INTERNAL "NRP Plugin Installation Directory")

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
	    ${BOOST_PYTHON}
		cxxopts::cxxopts
		nlohmann_json::nlohmann_json
		pistache
		PkgConfig::LibZip
		Python3::Module
		Python3::Python
		restclient-cpp
		spdlog::spdlog

	PRIVATE
)

target_include_directories(${LIBRARY_NAME} BEFORE
	PUBLIC 
	    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"

	PRIVATE
)


##########################################
## NRPGeneralPythonModule
if(NOT ${PYTHON_MODULE_SRC_FILES} STREQUAL "")
	add_library(${PYTHON_MODULE_NAME} SHARED ${PYTHON_MODULE_SRC_FILES})
	add_library(${NAMESPACE_NAME}::${PYTHON_MODULE_NAME} ALIAS ${PYTHON_MODULE_NAME})
	target_compile_options(${PYTHON_MODULE_NAME} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
	set_target_properties(${PYTHON_MODULE_NAME} PROPERTIES PREFIX "")

	target_include_directories(${PYTHON_MODULE_NAME}
		PUBLIC
	)

    target_link_libraries(${PYTHON_MODULE_NAME}
		PUBLIC
		    ${NAMESPACE_NAME}::${LIBRARY_NAME}
	)
endif()


##########################################
## Executable
if(NOT "${EXEC_SRC_FILES}" STREQUAL "")
	add_executable(${EXECUTABLE_NAME} ${EXEC_SRC_FILES})
	target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})
endif()


##########################################
## Append Doxygen files
add_doxygen_source("${CMAKE_CURRENT_SOURCE_DIR}" ${EXEC_SRC_FILES} ${LIB_SRC_FILES} ${PYTHON_MODULE_SRC_FILES} "${HEADER_DIRECTORY}")


##########################################
## Tests
if(${ENABLE_TESTING} AND NOT "${TEST_SRC_FILES}" STREQUAL "")
	# Python Module for testing
	set(TEST_PYTHON_MODULE_NAME "NRPGeneralPythonTestModule")

	add_library(${TEST_PYTHON_MODULE_NAME} SHARED "tests/python_module.cpp" "tests/test_transceiver_function_interpreter.cpp")
	set_target_properties(${TEST_PYTHON_MODULE_NAME} PROPERTIES PREFIX "")
	target_link_libraries(${TEST_PYTHON_MODULE_NAME} ${NAMESPACE_NAME}::${LIBRARY_NAME})

	# Create testing process executble (used for testing ProcessLaunchers)
	set(TEST_NRP_PROCESS_EXEC "TestNRPProcessExec")
	add_executable(${TEST_NRP_PROCESS_EXEC} tests/test_process_exec.cpp)
	target_link_libraries(${TEST_NRP_PROCESS_EXEC}
		PUBLIC
		${NAMESPACE_NAME}::${LIBRARY_NAME})

	# Create testing plugin library (used for testing PluginManager)
	set(TEST_NRP_PLUGIN "TestNRPPluginManager")
	add_library(${TEST_NRP_PLUGIN} SHARED tests/test_plugin.cpp)
	set_target_properties(${TEST_NRP_PLUGIN} PROPERTIES PREFIX "")
	target_link_libraries(${TEST_NRP_PLUGIN}
		PUBLIC
		${NAMESPACE_NAME}::${LIBRARY_NAME})

	# Create testing env files
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_files/test_env.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/test_env.sh" @ONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_env_cmake.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/tests/test_env_cmake.h" @ONLY)

	# Create testing executable
	enable_testing()
	add_executable(${TEST_NAME} ${TEST_SRC_FILES})
	target_link_libraries(${TEST_NAME}
		PUBLIC
		${NAMESPACE_NAME}::${LIBRARY_NAME}
		GTest::gtest
		GTest::gtest_main
		${TEST_PYTHON_MODULE_NAME})

	gtest_discover_tests(${TEST_NAME}
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
		EXTRA_ARGS -VV)
endif()


##########################################
## Installation

set(INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Install library files
install(TARGETS
		${LIBRARY_NAME}
	EXPORT
		${LIB_EXPORT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}

	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${HEADER_DIRECTORY}
)

# Install export target
install(EXPORT ${LIB_EXPORT_NAME}
	DESTINATION
		${INSTALL_CONFIGDIR}
	FILE
		"${LIB_EXPORT_NAME}.cmake"
	NAMESPACE
		"${NAMESPACE_NAME}::"
)

# Install headers
install(DIRECTORY "${HEADER_DIRECTORY}" "${CMAKE_CURRENT_BINARY_DIR}/include/${HEADER_DIRECTORY}"
	DESTINATION
	    ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING
	    PATTERN "*.h"
		PATTERN "*.hpp"
)

# Install python module
if(TARGET ${PYTHON_MODULE_NAME})
	install(TARGETS ${PYTHON_MODULE_NAME}
		DESTINATION "${PYTHON_INSTALL_DIR_REL}/${PYTHON_MODULE_NAME}")

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py"
		DESTINATION "${PYTHON_INSTALL_DIR_REL}/${PYTHON_MODULE_NAME}")
endif()

# Install executable files
if(TARGET ${EXECUTABLE_NAME})
	install(TARGETS ${EXECUTABLE_NAME}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# create cmake version and config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${LIB_VERSION_NAME}.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${LIB_CONFIG_NAME}.cmake"
	INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install cmake version and config files
install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${LIB_CONFIG_NAME}.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/${LIB_VERSION_NAME}.cmake"
	DESTINATION ${INSTALL_CONFIGDIR}
)

# Install env set script
install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/set_env_exec.sh"
	DESTINATION ${CMAKE_INSTALL_SBINDIR})
