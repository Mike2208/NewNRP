set(PROJECT_NAME "NRPSimulation")
set(HEADER_DIRECTORY "nrp_simulation")

set(NAMESPACE_NAME "${PROJECT_NAME}")

set(LIBRARY_NAME "NRPSimulationLib")
set(PYTHON_MODULE_NAME "Py${PROJECT_NAME}")
set(EXECUTABLE_NAME "NRPSimulation")
set(TEST_NAME "${PROJECT_NAME}Tests")

set(LIB_EXPORT_NAME "${LIBRARY_NAME}Targets")
set(LIB_CONFIG_NAME "${LIBRARY_NAME}Config")
set(LIB_VERSION_NAME "${LIB_CONFIG_NAME}Version")

cmake_minimum_required(VERSION 3.12)
project("${PROJECT_NAME}" VERSION 1.0.0)

include(GNUInstallDirs)

# List library build files
set(LIB_SRC_FILES
	nrp_simulation/config/server_config.cpp
	#nrp_simulation/server/simulation_server.cpp
	#nrp_simulation/server/simulation_status.cpp
	nrp_simulation/simulation/simulation_loop.cpp
	nrp_simulation/simulation/simulation_manager.cpp
	nrp_simulation/utils/pipe_packet_communication.cpp
)

# List of python module build files
set(PYTHON_MODULE_SRC_FILES
)

# List executable build files
set(EXEC_SRC_FILES
	nrp_simulation_executable/main.cpp
)

# List testing build files
set(TEST_SRC_FILES
    tests/simulation_loop.cpp
	tests/simulation_manager.cpp
)


##########################################
## Dependencies

# pistache: REST server library
find_package(Pistache REQUIRED)


##########################################
## Header configuration

include_directories("${CMAKE_CURRENT_BINARY_DIR}/../proto")

# Server config file name
set(NRP_SERVER_CONFIG_FILE_NAME "nrp.conf")

# Set directories under which to look for NRP Server Config File
set(NRP_GLOBAL_SERVER_CONFIG_DIR "${CMAKE_INSTALL_PREFIX}/etc/nrp/")
set(NRP_SERVER_CONFIG_DIRS "~/.config/nrp/" "${NRP_GLOBAL_SERVER_CONFIG_DIR}" "/etc/nrp/")
list(JOIN NRP_SERVER_CONFIG_DIRS "\", \"" NRP_SERVER_CONFIG_DIRS_STR)

# Default working directory
set(NRP_SERVER_DEFAULT_WORKING_DIR "${CMAKE_INSTALL_PREFIX}/var/nrp/" CACHE PATH "Default working directory of the NRP")

# Default experiment max
set(NRP_SERVER_DEF_MAX_NUM_EXPERIMENTS 5 CACHE STRING "Default server experiment limit")

# Default engines
list(JOIN NRP_SIMULATION_DEFAULT_ENGINE_LAUNCHERS "\", \"" NRP_SIMULATION_DEFAULT_ENGINE_LAUNCHERS_STR)

# General Header defines
configure_file(nrp_simulation/config/cmake_conf.h.in ${CMAKE_CURRENT_SOURCE_DIR}/nrp_simulation/config/cmake_conf.h @ONLY)


##########################################
# NRPServer
add_library(${LIBRARY_NAME} SHARED ${LIB_SRC_FILES})
add_library(${NAMESPACE_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
	${NRP_GEN_LIB_TARGET}
	pistache
)

target_include_directories(${LIBRARY_NAME}
	PUBLIC 
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>		
		${PYTHON_INCLUDE_DIR}

	PRIVATE
)


##########################################
## NRPServerPythonModule
if(NOT ${PYTHON_MODULE_SRC_FILES} STREQUAL "")
	add_library(${PYTHON_MODULE_NAME} SHARED ${PYTHON_MODULE_SRC_FILES})
	add_library(${NAMESPACE_NAME}::${PYTHON_MODULE_NAME} ALIAS ${PYTHON_MODULE_NAME})
	target_compile_options(${PYTHON_MODULE_NAME} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
	set_target_properties(${PYTHON_MODULE_NAME} PROPERTIES PREFIX "")

	target_include_directories(${PYTHON_MODULE_NAME}
		PUBLIC
		    $<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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
	target_link_libraries(${EXECUTABLE_NAME}
		${NAMESPACE_NAME}::${LIBRARY_NAME}
	)
endif()


##########################################
## Append Doxygen files
add_doxygen_source("${CMAKE_CURRENT_SOURCE_DIR}" ${EXEC_SRC_FILES} ${LIB_SRC_FILES} ${PYTHON_MODULE_SRC_FILES} "${HEADER_DIRECTORY}")


##########################################
## Server Configuration Generator
add_executable(NRPGenServerConf nrp_gen_server_conf/nrp_gen_server_conf.cpp)
target_link_libraries(NRPGenServerConf
	${NAMESPACE_NAME}::${LIBRARY_NAME}
)

add_custom_target(NRPServerConf ALL
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/NRPGenServerConf "${CMAKE_CURRENT_BINARY_DIR}/${NRP_SERVER_CONFIG_FILE_NAME}"
	DEPENDS NRPGenServerConf
	BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/${NRP_SERVER_CONFIG_FILE_NAME}"
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Creating default server config file"
	VERBATIM)


##########################################
## Tests
if(${ENABLE_TESTING} AND NOT "${TEST_SRC_FILES}" STREQUAL "")
	set(GAZEBO_WORLD_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_files/empty_world.sdf")

	# Create testing env files
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_files/simulation_config.json.in"
		"${CMAKE_CURRENT_SOURCE_DIR}/tests/test_files/simulation_config.json" @ONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_files/test_env.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/test_env.sh" @ONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_env_cmake.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_env_cmake.h" @ONLY)

	# Create testing executable
	enable_testing()
	add_executable(${TEST_NAME} ${TEST_SRC_FILES})
	target_link_libraries(${TEST_NAME}
		PUBLIC
		${NAMESPACE_NAME}::${LIBRARY_NAME}
		NRPNestJSONEngine::NRPNestJSONEngine
		NRPGazeboJSONEngine::NRPGazeboJSONEngine
		GTest::GTest
		GTest::GTestMain)

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
install(DIRECTORY ${HEADER_DIRECTORY}
	DESTINATION
	    ${CMAKE_INSTALL_INCLUDEDIR}/${HEADER_DIRECTORY}
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

# Install server conf
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NRP_SERVER_CONFIG_FILE_NAME}
	DESTINATION ${NRP_GLOBAL_SERVER_CONFIG_DIR}
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# Install server working directory
if(${NRP_SERVER_DEFAULT_WORKING_DIR} STREQUAL "")
	install(DIRECTORY DESTINATION ${NRP_SERVER_DEFAULT_WORKING_DIR})
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
