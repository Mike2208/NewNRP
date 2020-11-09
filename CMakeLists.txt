cmake_minimum_required(VERSION 3.12)
project(NRProject VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 20)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
#string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")

include(GNUInstallDirs)
include(ExternalProject)


##########################################
## Unit testing library
option(ENABLE_TESTING "Build tests" ON)
if(${ENABLE_TESTING})
	option(INSTALL_GTEST "Install google testing framework" OFF)
	add_subdirectory("depends/googletest")
	add_library(GTest::GTest ALIAS gtest)
	add_library(GTest::GTestMain ALIAS gtest_main)

	include(GoogleTest)
endif()


##########################################
## Python 3 libraries
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(PythonLibs 3 REQUIRED)

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
option(SPDLOG_BUILD_SHARED "Create shared libraries for spdlog" ON)
option(SPDLOG_INSTALL "Configure spdlog install" ON)
add_subdirectory(depends/spdlog)


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
add_subdirectory(nrp_python_device_interface)

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
get_property(DOXYGEN_SOURCES GLOBAL PROPERTY DOXYGEN_SOURCE_LIST)
doxygen_add_docs(doxygen
	${DOXYGEN_SOURCES}
	"docs/main_page.dox" "docs/engine_types.dox"
	COMMENT "Generating documentation")
