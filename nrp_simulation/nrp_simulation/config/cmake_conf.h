#ifndef CMAKE_CONF_H
#define CMAKE_CONF_H

/* Automatically created by cmake configure file from nrp_simulation/cmake_conf.h.in
*  DO NOT EDIT DIRECTLY!
*/

/*!
 * \brief Path to NRP simulation executable
 */
#define NRP_SIMULATION_EXECUTABLE "/usr/local/nrp/NRPSimulation"

/*!
 * \brief Engine Launcher plugins that should be added to server by default
 */
#define NRP_SIMULATION_DEFAULT_ENGINE_LAUNCHERS {"NRPGazeboJSONEngine.so", "NRPNestJSONEngine.so"}

/*!
 * \brief Server config file name
 */
#define NRP_SERVER_CONFIG_FILE_NAME "nrp.conf"

/*!
 * \brief NRP Server default working directory
 */
#define NRP_SERVER_DEFAULT_WORKING_DIR "/usr/local/nrp/var/nrp"

/*!
 * \brief Default limit for experiments
 */
#define NRP_SERVER_DEF_MAX_NUM_EXPERIMENTS 5

/*!
 * \brief Directories under which to search for config file
 */
#define NRP_SERVER_CONFIG_DIRS  {"~/.config/nrp/", "/usr/local/nrp/etc/nrp/", "/etc/nrp/"}

#endif // CMAKE_CONF_H
