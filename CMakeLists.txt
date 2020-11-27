cmake_minimum_required(VERSION 3.16)
project(NRProject VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 20)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
#string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")

include(GNUInstallDirs)
include(ExternalProject)
include(FetchContent)

cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

##########################################
## Unit testing library
option(ENABLE_TESTING "Build tests" ON)
if(${ENABLE_TESTING})
	include(GoogleTest)

	find_package(GTest 1.10 QUIET)
	if(NOT ${GTest_FOUND})
		message("Please wait. Downloading GTest...")
		FetchContent_Declare(GTest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG v1.10.x)

		FetchContent_GetProperties(GTest)
		if(NOT GTest_POPULATED)
			FetchContent_Populate(GTest)

			set(INSTALL_GTEST OFF)
			add_subdirectory(${gtest_SOURCE_DIR} ${gtest_BINARY_DIR})

			add_library(GTest::gtest ALIAS gtest)
			add_library(GTest::gtest_main ALIAS gtest_main)
		endif()
	endif()
endif()


##########################################
## Python 3 libraries
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

add_compile_definitions(BOOST_ASIO_DISABLE_CONCEPTS)
find_package(Boost REQUIRED)
if(Boost_VERSION VERSION_GREATER_EQUAL 1.71)
	set(BOOST_PYTHON_COMPONENT python)
else()
	set(BOOST_PYTHON_COMPONENT python3)
endif()
find_package(Boost REQUIRED COMPONENTS ${BOOST_PYTHON_COMPONENT} filesystem)
set(BOOST_PYTHON "Boost::${BOOST_PYTHON_COMPONENT}")

set(PYTHON_INSTALL_DIR_REL "python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages" CACHE INTERNAL "Relative install location for python libraries")
set(PYTHON_INSTALL_DIR_ABS "${CMAKE_INSTALL_LIBDIR}/${PYTHON_INSTALL_DIR_REL}" CACHE INTERNAL "Absoulte install location for python libraries")


##########################################
## Log library
find_package(spdlog 1.8.1 QUIET)
if(NOT ${spdlog_FOUND})
	message("Please wait. Downloading spdlog...")
	FetchContent_Declare(spdlog
		GIT_REPOSITORY https://github.com/gabime/spdlog.git
		GIT_TAG v1.8.1)

	FetchContent_GetProperties(spdlog)
	if(NOT spdlog_POPULATED)
		FetchContent_Populate(spdlog)

		set(SPDLOG_BUILD_SHARED ON)
		set(SPDLOG_INSTALL ON)

		add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
	endif()
endif()


##########################################
## Doxygen
find_package(Doxygen REQUIRED)
set_property(GLOBAL PROPERTY DOXYGEN_SOURCE_LIST)
function(add_doxygen_source directory)
	set(NUM_SOURCES ${ARGC}-1)
	list(SUBLIST ARGV 1 ${NUM_SOURCES} ADD_SOURCES)
	get_property(tmp GLOBAL PROPERTY DOXYGEN_SOURCE_LIST)
	foreach(arg ${ADD_SOURCES})
		set(tmp ${tmp} "${directory}/${arg}")
	endforeach()
	set_property(GLOBAL PROPERTY DOXYGEN_SOURCE_LIST "${tmp}")
endfunction(add_doxygen_source)

add_subdirectory(proto)

##########################################
## General NRP library
## Includes components to create new devices and engines
add_subdirectory(nrp_general_library)


##########################################
## NRP Communication Protocols
add_subdirectory(nrp_base_engines/nrp_json_engine_base)


##########################################
## NRP Engines

## Gazebo JSON Engine
add_subdirectory(nrp_gazebo_json_engine)

## Gazebo gRPC Engine
add_subdirectory(nrp_gazebo_grpc_engine)

## Gazebo MPI Engine
add_subdirectory(nrp_gazebo_mpi_engine)

## Nest Engine
add_subdirectory(nrp_nest_json_engine)
add_subdirectory(nrp_nest_server_engine)

## Python Engine
add_subdirectory(nrp_python_json_engine)


##########################################
## NRP Simulation
set(NRP_SIMULATION_DEFAULT_ENGINE_LAUNCHERS "NRPGazeboGrpcEngine.so;NRPNestJSONEngine.so"
	CACHE STRING "Default engines which will always be available in the NRP server")
add_subdirectory(nrp_simulation)

##########################################
## NRP Server
#add_subdirectory(nrp_server)

##########################################
## NRP Client
#add_subdirectory(nrp_client)


##########################################
## Doxygen
set(DOXYGEN_GENERATE_LATEX YES)
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_EXAMPLE_PATH
	${DOXYGEN_EXAMPLE_PATH}
	"docs/examples"
	"docs/example_engine")

add_subdirectory("docs/example_engine" EXCLUDE_FROM_ALL)
add_subdirectory("docs/example_json_engine" EXCLUDE_FROM_ALL)

get_property(DOXYGEN_SOURCES GLOBAL PROPERTY DOXYGEN_SOURCE_LIST)
doxygen_add_docs(doxygen
	${DOXYGEN_SOURCES}
	"docs/engine_types.dox"
	"docs/main_page.dox"
	"docs/todos.dox"
	"docs/tutorials/engine_creation.dox"
	"docs/tutorials/grpc_engine_creation.dox"
	"docs/tutorials/json_engine_creation.dox"
	COMMENT "Generating documentation")
