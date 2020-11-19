set(PROJECT_NAME "NRPGazeboMPIEngine")
set(HEADER_DIRECTORY "nrp_gazebo_mpi_engine")

set(NAMESPACE_NAME "${PROJECT_NAME}")

set(LIBRARY_NAME "${PROJECT_NAME}")
set(PYTHON_MODULE_NAME "${PROJECT_NAME}Python")
set(EXECUTABLE_NAME "${PROJECT_NAME}Main")
set(TEST_NAME "${PROJECT_NAME}Tests")

set(LIB_EXPORT_NAME "${LIBRARY_NAME}Targets")
set(LIB_CONFIG_NAME "${LIBRARY_NAME}Config")
set(LIB_VERSION_NAME "${LIB_CONFIG_NAME}Version")

cmake_minimum_required(VERSION 3.16)
project("${PROJECT_NAME}" VERSION 1.0.0)

include(GNUInstallDirs)

# List library build files
set(LIB_SRC_FILES
	nrp_gazebo_mpi_engine/config/gazebo_config.cpp
	nrp_gazebo_mpi_engine/engine_server/gazebo_step_controller.cpp
	nrp_gazebo_mpi_engine/nrp_client/nrp_gazebo_mpi_client.cpp
	nrp_gazebo_mpi_engine/devices/physics_camera.cpp
	nrp_gazebo_mpi_engine/devices/physics_joint.cpp
	nrp_gazebo_mpi_engine/devices/physics_link.cpp
)

# List of python module build files
set(PYTHON_MODULE_SRC_FILES
	nrp_gazebo_mpi_engine/python/python_module.cpp
)

# List executable build files
set(EXEC_SRC_FILES
)

# List testing build files
set(TEST_SRC_FILES
	tests/test_gazebo_engine.cpp
)


##########################################
## Dependencies
find_package(gazebo REQUIRED)
find_package(MPI REQUIRED)


##########################################
## Header configuration
set(NRP_GAZEBO_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/nrp_gazebo_plugins" CACHE PATH "Install location of gazebo plugins")

set(NRP_GAZEBO_COMMUNICATION_PLUGIN NRPGazeboMPICommunicationPlugin)

set(NRP_GAZEBO_PLUGINS_INSTALL_DIR_ABSOLUTE ${NRP_GAZEBO_PLUGINS_INSTALL_DIR})
if(NOT IS_ABSOLUTE ${NRP_GAZEBO_PLUGINS_INSTALL_DIR})
	set(NRP_GAZEBO_PLUGINS_INSTALL_DIR_ABSOLUTE ${CMAKE_INSTALL_PREFIX}/${NRP_GAZEBO_PLUGINS_INSTALL_DIR})
endif()

list(JOIN GAZEBO_LIBRARY_DIRS ":" DEFAULT_GAZEBO_LIB_DIRS)

# Gazebo defines
configure_file("nrp_gazebo_mpi_engine/config/cmake_constants.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/${HEADER_DIRECTORY}/config/cmake_constants.h" @ONLY)

# Python module dependencies
configure_file("nrp_gazebo_mpi_engine/python/__init__.py.in" "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py" @ONLY)

##########################################
# NRPGazeboMPIEngine
add_library("${LIBRARY_NAME}" SHARED ${LIB_SRC_FILES})
add_library(${NAMESPACE_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "")

target_include_directories(${LIBRARY_NAME}
	PUBLIC 
	    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
)

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
	    ${NRP_GEN_LIB_TARGET}
		MPI::MPI_CXX
)


##########################################
## NRPGazeboDevicePythonModule
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
	add_executable("${EXECUTABLE_NAME}" ${EXEC_SRC_FILES})
	target_link_libraries("${EXECUTABLE_NAME}" "${LIBRARY_NAME}")
endif()


##########################################
## Append Doxygen files
add_doxygen_source("${CMAKE_CURRENT_SOURCE_DIR}" ${EXEC_SRC_FILES} ${LIB_SRC_FILES} ${PYTHON_MODULE_SRC_FILES} ${HEADER_DIRECTORY})


##########################################
## Gazebo plugins
set(NRP_GAZEBO_LIB_TARGET "${NAMESPACE_NAME}::${LIBRARY_NAME}")
add_subdirectory(gazebo_plugins)


##########################################
## Tests
if(${ENABLE_TESTING} AND NOT "${TEST_SRC_FILES}" STREQUAL "")
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
		NRPGazeboPlugins::NRPGazeboJointControllerPlugin
		NRPGazeboPlugins::NRPGazeboLinkControllerPlugin
		NRPGazeboPlugins::NRPGazeboCameraControllerPlugin
		NRPGazeboPlugins::NRPGazeboWorldPlugin
		"NRPGazeboPlugins::${NRP_GAZEBO_COMMUNICATION_PLUGIN}")

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
	LIBRARY DESTINATION ${NRP_PLUGIN_INSTALL_DIR}
	ARCHIVE DESTINATION ${NRP_PLUGIN_INSTALL_DIR}
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
		DESTINATION "${PYTHON_INSTALL_DIR_ABS}/${PYTHON_MODULE_NAME}")

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py"
		DESTINATION "${PYTHON_INSTALL_DIR_ABS}/${PYTHON_MODULE_NAME}")
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
