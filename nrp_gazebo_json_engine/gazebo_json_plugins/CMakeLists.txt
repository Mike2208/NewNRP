cmake_minimum_required(VERSION 3.16)
project(NRPGazeboJSONPlugins VERSION 0.0.1 LANGUAGES CXX)

include(GNUInstallDirs)

find_package(gazebo REQUIRED)
find_package(Pistache REQUIRED)
find_package(nlohmann_json REQUIRED)

link_directories(${GAZEBO_LIBRARY_DIRS})
#list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

if(NOT DEFINED NRP_GAZEBO_COMMUNICATION_PLUGIN)
	set(NRP_GAZEBO_COMMUNICATION_PLUGIN NRPGazeboJSONCommunicationPlugin)
endif()

########################################
## Gazebo Communication Plugin
add_library(${NRP_GAZEBO_COMMUNICATION_PLUGIN} SHARED "nrp_communication_controller/nrp_communication_controller.cpp" "nrp_communication_controller/nrp_communication_plugin.cpp")
add_library("NRPGazeboJSONPlugins::${NRP_GAZEBO_COMMUNICATION_PLUGIN}" ALIAS ${NRP_GAZEBO_COMMUNICATION_PLUGIN})
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
		${NRP_GEN_LIB_TARGET}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo World Plugin
add_library(NRPGazeboJSONWorldPlugin SHARED "nrp_world_plugin/nrp_world_plugin.cpp")
add_library(NRPGazeboJSONPlugins::NRPGazeboJSONWorldPlugin ALIAS NRPGazeboJSONWorldPlugin)
target_compile_options(NRPGazeboJSONWorldPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
target_include_directories(NRPGazeboJSONWorldPlugin
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>

	PRIVATE
	    ${NRP_GAZEBO_LIB_TARGET}
)

target_link_libraries(NRPGazeboJSONWorldPlugin
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo Joint Controller Plugin
add_library(NRPGazeboJSONJointControllerPlugin SHARED "nrp_joint_controller/nrp_joint_controller.cpp")
add_library(NRPGazeboJSONPlugins::NRPGazeboJSONJointControllerPlugin ALIAS NRPGazeboJSONJointControllerPlugin)
target_compile_options(NRPGazeboJSONJointControllerPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

target_include_directories(NRPGazeboJSONJointControllerPlugin
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>
)

target_link_libraries(NRPGazeboJSONJointControllerPlugin
	PUBLIC
	    ${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo Link Controller Plugin
add_library(NRPGazeboJSONLinkControllerPlugin SHARED "nrp_link_controller_plugin/nrp_link_controller_plugin.cpp")
add_library(NRPGazeboJSONPlugins::NRPGazeboJSONLinkControllerPlugin ALIAS NRPGazeboJSONLinkControllerPlugin)
target_compile_options(NRPGazeboJSONLinkControllerPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)
add_library(NRPGazeboJSONPlugins::NRPGazeboJSONLinkControllerPlugin ALIAS NRPGazeboJSONLinkControllerPlugin)

target_include_directories(NRPGazeboJSONLinkControllerPlugin
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>
)

target_link_libraries(NRPGazeboJSONLinkControllerPlugin
	PUBLIC
	    nlohmann_json::nlohmann_json
		${NRP_GAZEBO_LIB_TARGET}

	PRIVATE
	    ${NRP_GEN_LIB_TARGET}
		${NRP_GAZEBO_COMMUNICATION_PLUGIN}
		${GAZEBO_LIBRARIES}
)


########################################
## Gazebo Camera Controller Plugin
add_library(NRPGazeboJSONCameraControllerPlugin SHARED "nrp_camera_controller_plugin/nrp_camera_controller_plugin.cpp")
add_library(NRPGazeboJSONPlugins::NRPGazeboJSONCameraControllerPlugin ALIAS NRPGazeboJSONCameraControllerPlugin)
target_compile_options(NRPGazeboJSONCameraControllerPlugin PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

target_include_directories(NRPGazeboJSONCameraControllerPlugin
	PUBLIC
	    $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${GAZEBO_INCLUDE_DIRS}>

	PRIVATE
	    ${NRP_GAZEBO_LIB_TARGET}
)

target_link_libraries(NRPGazeboJSONCameraControllerPlugin
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
set(PLUGIN_TARGETS ${NRP_GAZEBO_COMMUNICATION_PLUGIN} NRPGazeboJSONWorldPlugin
	NRPGazeboJSONJointControllerPlugin NRPGazeboJSONLinkControllerPlugin
	NRPGazeboJSONCameraControllerPlugin)

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
