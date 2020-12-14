set(PROJECT_NAME "NRPNestServerEngine")
set(HEADER_DIRECTORY "nrp_nest_server_engine")

set(NAMESPACE_NAME "${PROJECT_NAME}")

set(LIBRARY_NAME "${PROJECT_NAME}")
set(PYTHON_MODULE_NAME "NRPNestServerPythonModule")
set(EXECUTABLE_NAME "NRPNestServerExecutable")
set(TEST_NAME "${PROJECT_NAME}Tests")

set(LIB_EXPORT_NAME "${LIBRARY_NAME}Targets")
set(LIB_CONFIG_NAME "${LIBRARY_NAME}Config")
set(LIB_VERSION_NAME "${LIB_CONFIG_NAME}Version")

cmake_minimum_required(VERSION 3.16)
project("${PROJECT_NAME}" VERSION ${NRP_VERSION})

include(GNUInstallDirs)
include(ExternalProject)

# List library build files
set(LIB_SRC_FILES
	nrp_nest_server_engine/config/nest_server_config.cpp
	nrp_nest_server_engine/devices/nest_server_device.cpp
	nrp_nest_server_engine/nrp_client/nest_engine_server_nrp_client.cpp
	nrp_nest_server_engine/python/create_device_class.cpp
)

# List of python module build files
set(PYTHON_MODULE_SRC_FILES
	nrp_nest_server_engine/python/nrp_nest_python_module.cpp
)

# List executable build files
set(EXEC_SRC_FILES
)

# List testing build files
set(TEST_SRC_FILES
	tests/test_nest_server.cpp
)


##########################################
## Dependencies

# NEST Simulator
if("${NEST_BUILD_DIR}" STREQUAL "")
	set(NEST_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/nest-simulator" CACHE INTERNAL "Nest build dir")
endif()
if(NOT TARGET nest-simulator)
	set(NRP_NEST_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
	ExternalProject_Add(nest-simulator
		GIT_REPOSITORY https://github.com/nest/nest-simulator.git
		GIT_TAG c354d429e60ddb35a61ad02e3a070477e5026fa8
		GIT_PROGRESS true

		PREFIX ${NEST_BUILD_DIR}
		INSTALL_DIR ${CMAKE_INSTALL_PREFIX}

		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${NRP_NEST_INSTALL_PREFIX} -Dwith-mpi=ON -Dwith-python=ON

		UPDATE_COMMAND ""
	)
endif()


# Workaround for uwsgi bug. Currently, the nest-server only work with uwsgi installed via pip. It fails
# when using uwsgi installed with apt. This creates and installs a workaround installation.
# Look at https://github.com/nest/nest-simulator/pull/1873 and https://github.com/nest/nest-simulator/issues/1872
# for upstream status. Once the PR was merged, update the nest-simulator GIT-TAG here and in nrp_nest_json_engine/CMakeLists.txt,
# then remove this workaround section and uncomment set(NRP_NEST_SERVER_EXECUTABLE ... in section Header Configuration right below
## Begin workaround
add_custom_target(nest-server-uwsgi-bin ALL sh -c
	"sed 's/uwsgi --module nest\.server:app/ uwsgi --plugin python3 --module nest.server:app/' '${NEST_BUILD_DIR}/src/nest-simulator/extras/nest-server' > '${CMAKE_CURRENT_BINARY_DIR}/nest-server-uwsgi'"
	DEPENDS nest-simulator
	BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/nest-server-uwsgi"
	VERBATIM)
set(NRP_NEST_SERVER_EXECUTABLE "nest-server-uwsgi")
install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/nest-server-uwsgi"
	DESTINATION "${CMAKE_INSTALL_BINDIR}"
	PERMISSIONS
		OWNER_READ OWNER_EXECUTE OWNER_WRITE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE
)
## End workaround


##########################################
## Header configuration

# General Nest Header defines
set(NEST_CMAKE_HEADER_CONFIG_FILE "nrp_nest_server_engine/config/cmake_constants.h.in")
set(NRP_NEST_CMAKE_INSTALL_BINDIR "${NRP_NEST_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
#set(NRP_NEST_SERVER_EXECUTABLE "nest-server")																				Comment out after fixing above workaround
set(NEST_CMAKE_INSTALL_PYNEST "${NEST_CMAKE_INSTALL_PREFIX}/lib/python3.6/site-packages")
configure_file(${NEST_CMAKE_HEADER_CONFIG_FILE} "${CMAKE_CURRENT_BINARY_DIR}/include/${HEADER_DIRECTORY}/config/cmake_constants.h" @ONLY)

# Python module dependencies
configure_file("nrp_nest_server_engine/python/__init__.py.in" "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py" @ONLY)


##########################################
# NRPNestDeviceInterface
add_library("${LIBRARY_NAME}" SHARED ${LIB_SRC_FILES})
add_library(${NAMESPACE_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<CXX_COMPILER_ID:GNU>:-fconcepts>)

set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "")

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
	    NRPJSONEngineProtocol::NRPJSONEngineProtocol

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
## NRPNestPythonModule
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
## NRPNestExecutable
if(NOT "${EXEC_SRC_FILES}" STREQUAL "")
	add_executable(${EXECUTABLE_NAME} ${EXEC_SRC_FILES})
	target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})
endif()


##########################################
## Append Doxygen files
add_doxygen_source("${CMAKE_CURRENT_SOURCE_DIR}" ${EXEC_SRC_FILES} ${LIB_SRC_FILES} ${PYTHON_MODULE_SRC_FILES} ${HEADER_DIRECTORY})


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
		GTest::gtest_main)

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

# Fix nest-server to use package manager's uwsgi plugin system
install(CODE "execute_process(COMMAND sed -i 's/uwsgi --module nest\.server:app/ uwsgi --plugin python --module nest.server:app/' nest-server
	WORKING_DIRECTORY '${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}')")

# Install python module
if(TARGET ${PYTHON_MODULE_NAME})
	install(TARGETS ${PYTHON_MODULE_NAME}
		DESTINATION "${PYTHON_INSTALL_DIR_REL}/${PYTHON_MODULE_NAME}")

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py"
		DESTINATION "${PYTHON_INSTALL_DIR_REL}/${PYTHON_MODULE_NAME}")

	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/nrp_nest_server_engine/python/numpy_json_serializer.py"
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
