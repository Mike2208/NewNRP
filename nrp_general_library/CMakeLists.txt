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

cmake_minimum_required(VERSION 3.12)
project("${PROJECT_NAME}" VERSION 1.0.0)

include(GNUInstallDirs)

# List library build files
set(LIB_SRC_FILES
	nrp_general_library/config/base_config.cpp
	nrp_general_library/config/config_formats/json_config_properties.cpp
	nrp_general_library/config/config_storage.cpp
	nrp_general_library/config/engine_config.cpp
	nrp_general_library/config/simulation_config.cpp
	nrp_general_library/config/transceiver_function_config.cpp
	nrp_general_library/device_interface/device_conversion_mechanism.cpp
	nrp_general_library/device_interface/device_interface.cpp
	nrp_general_library/device_interface/devices/python_object_device_interface.cpp
	nrp_general_library/device_interface/python_device.cpp
	nrp_general_library/engine_interfaces/engine_interface.cpp
	nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_device_controller.cpp
	nrp_general_library/engine_interfaces/engine_grpc_interface/engine_server/engine_grpc_server.cpp
	nrp_general_library/engine_interfaces/engine_grpc_interface/engine_client/engine_grpc_client.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/config/engine_json_config.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/device_interfaces/json_device_conversion_mechanism.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_device_controller.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_server.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_opts_parser.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/nrp_client/engine_json_nrp_client.cpp
	nrp_general_library/engine_interfaces/engine_json_interface/nrp_client/engine_json_registration_server.cpp
	nrp_general_library/engine_interfaces/engine_launcher_manager.cpp
	nrp_general_library/plugin_system/plugin.cpp
	nrp_general_library/plugin_system/plugin_manager.cpp
	nrp_general_library/process_launchers/process_launcher.cpp
	nrp_general_library/process_launchers/process_launcher_basic.cpp
	nrp_general_library/process_launchers/process_launcher_manager.cpp
	nrp_general_library/transceiver_function/single_transceiver_device.cpp
	nrp_general_library/transceiver_function/transceiver_function.cpp
	nrp_general_library/transceiver_function/transceiver_device_interface.cpp
	nrp_general_library/transceiver_function/transceiver_function_interpreter.cpp
	nrp_general_library/transceiver_function/transceiver_function_manager.cpp
	nrp_general_library/transceiver_function/transceiver_function_sorted_results.cpp
	nrp_general_library/utils/property_template.cpp
	nrp_general_library/utils/ptr_templates.cpp
	nrp_general_library/utils/restclient_setup.cpp
	nrp_general_library/utils/serializers/json_property_serializer.cpp
	nrp_general_library/utils/serializers/property_serializer.cpp
	nrp_general_library/utils/serializers/property_serializer_general.cpp
	nrp_general_library/utils/serializers/python_dict_property_serializer.cpp
	nrp_general_library/utils/file_finder.cpp
	nrp_general_library/utils/fixed_string.cpp
	nrp_general_library/utils/pipe_communication.cpp
	nrp_general_library/utils/python_error_handler.cpp
	nrp_general_library/utils/python_interpreter_state.cpp
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
	tests/engine_json_config.cpp
	tests/engine_json_server.cpp
	tests/engine_json_nrp_client.cpp
	tests/engine_launcher_manager.cpp
	tests/engine_grpc.cpp
	tests/json_device_methods.cpp
	tests/json_property_serializer.cpp
	tests/plugin_manager.cpp
	tests/property_template.cpp
	tests/python_dict_property_serializer.cpp
	tests/test_process_launcher_basic.cpp
	tests/transceiver_function_interpreter.cpp
)

# List of protobuf proto files
get_filename_component(hw_proto "nrp_general_library/proto/engine_grpc.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

##########################################
## Dependencies

# cxxopts: Start Parameter Processing
add_subdirectory(depends/cxxopts)

# restclient-cpp: REST client library
add_subdirectory(depends/restclient-cpp)
add_library(restclient-cpp::restclient-cpp ALIAS restclient-cpp)

# pistache: REST server library
find_package(Pistache REQUIRED)

# Protobuf: google's Protocol Buffers library
pkg_check_modules(PROTOBUF REQUIRED protobuf)
message(STATUS "Using protobuf ${PROTOBUF_VERSION}")

# Protobuf compiler
find_program(PROTOC protoc)
message(STATUS "Using protoc ${PROTOC}")

# gRPC cpp plugin
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
message(STATUS "Using gRPC cpp plugin ${GRPC_CPP_PLUGIN}")

# gRPC: google's Remote Procedure Calls library
pkg_check_modules(GRPC REQUIRED grpc++)
message(STATUS "Using gRPC ${GRPC_VERSION}")

# nlohmann_json: JSON library
find_package(nlohmann_json 3.9.0)
if(NOT ${nlohmann_json_FOUND})
	message("Please wait. Downloading and compiling a newer version of nlohmann_json...")
	include(FetchContent)
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

# Generated sources
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/engine_grpc.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/engine_grpc.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/engine_grpc.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/engine_grpc.grpc.pb.h")
add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ${PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
        "${hw_proto}"
	DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(LIB_SRC_FILES ${LIB_SRC_FILES} ${proto_srcs} ${grpc_srcs})

##########################################
## Header configuration

# General Header defines
configure_file("nrp_general_library/config_headers/nrp_cmake_constants.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/nrp_general_library/config_headers/nrp_cmake_constants.h" @ONLY)

# Python Module defines
configure_file("nrp_python_device/__init__.py.in" "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py" @ONLY)


##########################################
# NRPGeneralLibrary
add_library("${LIBRARY_NAME}" SHARED ${LIB_SRC_FILES})
add_library(${NAMESPACE_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<CXX_COMPILER_ID:GNU>:-fconcepts>)

set(NRP_GEN_LIB_TARGET "${NAMESPACE_NAME}::${LIBRARY_NAME}" CACHE INTERNAL "NRP General Library target")
set(NRP_PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" CACHE INTERNAL "NRP Plugin Installation Directory")

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
	    ${BOOST_PYTHON}
		cxxopts::cxxopts
		nlohmann_json::nlohmann_json
		pistache
		PkgConfig::LibZip
		${PYTHON_LIBRARIES}
		restclient-cpp::restclient-cpp
		spdlog::spdlog
		${PROTOBUF_LIBRARIES}
		${GRPC_LIBRARIES}

	PRIVATE
)

target_include_directories(${LIBRARY_NAME}
	PUBLIC 
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>		
		${PYTHON_INCLUDE_DIR}

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
	add_library(${TEST_NRP_PLUGIN} tests/test_plugin.cpp)
	set_target_properties(${TEST_NRP_PLUGIN} PROPERTIES PREFIX "")
	target_link_libraries(${TEST_NRP_PLUGIN}
		PUBLIC
		${NAMESPACE_NAME}::${LIBRARY_NAME})

	# Create testing env files
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_files/test_env.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/test_env.sh" @ONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tests/test_env_cmake.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_env_cmake.h" @ONLY)

	# Create testing executable
	enable_testing()
	add_executable(${TEST_NAME} ${TEST_SRC_FILES})
	target_link_libraries(${TEST_NAME}
		PUBLIC
		${NAMESPACE_NAME}::${LIBRARY_NAME}
		GTest::GTest
		GTest::GTestMain
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
		DESTINATION "${PYTHON_INSTALL_DIR}/${PYTHON_MODULE_NAME}")

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py"
		DESTINATION "${PYTHON_INSTALL_DIR}/${PYTHON_MODULE_NAME}")
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
install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/set_env_exec.sh"
	DESTINATION ${CMAKE_INSTALL_BINDIR})
