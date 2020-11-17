cmake_minimum_required(VERSION 3.16)
project(NRPGazeboPlugins VERSION 0.0.1 LANGUAGES CXX)

include(GNUInstallDirs)

find_package(gazebo REQUIRED)
find_package(MPI REQUIRED)

link_directories(${GAZEBO_LIBRARY_DIRS})
#list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

if(NOT DEFINED NRP_GAZEBO_COMMUNICATION_PLUGIN)
	set(NRP_GAZEBO_COMMUNICATION_PLUGIN NRPGazeboMPICommunicationPlugin)
endif()

########################################
## Gazebo Communication Plugin
add_library(${NRP_GAZEBO_COMMUNICATION_PLUGIN} SHARED "nrp_communication_controller/nrp_communication_controller.cpp" "nrp_communication_controller/nrp_communication_plugin.cpp")
add_library("NRPGazeboPlugins::${NRP_GAZEBO_COMMUNICATION_PLUGIN}" ALIAS ${NRP_GAZEBO_COMMUNICATION_PLUGIN})
target_compile_options(${NRP_GAZEBO_COMMUNICATION_PLUGIN} PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

target_include_directories(${NRP_GAZEBO_COMMUNICATION_PLUGIN}
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>

	PRIVATE
)

target_link_libraries(${NRP_GAZEBO_COMMUNICATION_PLUGIN}
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    cxxopts::cxxopts
		MPI::MPI_CXX
		${NRP_GEN_LIB_TARGET}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo World Plugin
add_library(NRPGazeboWorldPlugin SHARED "nrp_world_plugin/nrp_world_plugin.cpp")
add_library(NRPGazeboPlugins::NRPGazeboWorldPlugin ALIAS NRPGazeboWorldPlugin)
target_compile_options(NRPGazeboWorldPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
target_include_directories(NRPGazeboWorldPlugin
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>

	PRIVATE
	    ${NRP_GAZEBO_LIB_TARGET}
)

target_link_libraries(NRPGazeboWorldPlugin
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo Joint Controller Plugin
add_library(NRPGazeboJointControllerPlugin SHARED "nrp_joint_controller/nrp_joint_controller.cpp")
add_library(NRPGazeboPlugins::NRPGazeboJointControllerPlugin ALIAS NRPGazeboJointControllerPlugin)
target_compile_options(NRPGazeboJointControllerPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

target_include_directories(NRPGazeboJointControllerPlugin
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>
)

target_link_libraries(NRPGazeboJointControllerPlugin
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo Link Controller Plugin
add_library(NRPGazeboLinkControllerPlugin SHARED "nrp_link_controller_plugin/nrp_link_controller_plugin.cpp")
add_library(NRPGazeboPlugins::NRPGazeboLinkControllerPlugin ALIAS NRPGazeboLinkControllerPlugin)
target_compile_options(NRPGazeboLinkControllerPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
add_library(NRPGazeboPlugins::NRPGazeboLinkControllerPlugin ALIAS NRPGazeboLinkControllerPlugin)

target_include_directories(NRPGazeboLinkControllerPlugin
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>
)

target_link_libraries(NRPGazeboLinkControllerPlugin
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo Camera Controller Plugin
add_library(NRPGazeboCameraControllerPlugin SHARED "nrp_camera_controller_plugin/nrp_camera_controller_plugin.cpp")
add_library(NRPGazeboPlugins::NRPGazeboCameraControllerPlugin ALIAS NRPGazeboCameraControllerPlugin)
target_compile_options(NRPGazeboCameraControllerPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

target_include_directories(NRPGazeboCameraControllerPlugin
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>

	PRIVATE
	    ${NRP_GAZEBO_LIB_TARGET}
)

target_link_libraries(NRPGazeboCameraControllerPlugin
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
		CameraPlugin
)


##########################################
## Append Doxygen files
add_doxygen_source("${CMAKE_CURRENT_SOURCE_DIR}" "")


########################################
## Configure targets
set(PLUGIN_TARGETS ${NRP_GAZEBO_COMMUNICATION_PLUGIN} NRPGazeboWorldPlugin
	NRPGazeboJointControllerPlugin NRPGazeboLinkControllerPlugin
	NRPGazeboCameraControllerPlugin)

set_target_properties(${PLUGIN_TARGETS}
	PROPERTIES PREFIX "")

list(JOIN PLUGIN_TARGETS "," PLUGIN_TARGETS_STRING)
set(NRP_GAZEBO_PLUGINS "${PLUGIN_TARGETS_STRING}"
	CACHE INTERNAL "List of compiled NRP Gazebo plugins")


########################################
## Installation
install(TARGETS	${PLUGIN_TARGETS}
	LIBRARY DESTINATION ${NRP_GAZEBO_PLUGINS_INSTALL_DIR}
	ARCHIVE DESTINATION ${NRP_GAZEBO_PLUGINS_INSTALL_DIR})
