//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include <gtest/gtest.h>

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/config/engine_mpi_config.h"
#include "nrp_general_library/process_launchers/launch_commands/mpi_spawn.h"
#include "nrp_general_library/utils/mpi_communication.h"
#include "nrp_general_library/utils/mpi_setup.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "tests/test_env_cmake.h"
#include "tests/test_mpi_communication_devices.h"

class TestConf : public EngineMPIConfig<TestConf, PropNames<> >
{
	public:
		static constexpr FixedString ConfigType = "test_mpi_conf";

		TestConf(EngineConfigConst::config_storage_t &stor)
		    : EngineMPIConfig(stor)
		{}
};

TEST(MPICommunication, TestComm)
{
	PythonInterpreterState pyState;
	MPISetup::initializeOnce(0, nullptr);

	EngineConfigConst::config_storage_t stor;
	TestConf conf(stor);
	conf.engineProcCmd() = TEST_MPI_COMMUNICATION_PROC;

	// Test process spawning and child PID receive
	MPISpawn spawnCmd;
	const pid_t mpiPID = spawnCmd.launchEngineProcess(conf, {}, {});
	ASSERT_EQ(mpiPID, spawnCmd.getEnginePID());

	MPI_Comm comm = spawnCmd.getIntercomm();

	// Test string receive
	const std::string testStr = MPICommunication::recvString(comm, MPI_ANY_TAG);
	const std::string testCompStr(TEST_MPI_COMM_STR);
	ASSERT_EQ(testStr, testCompStr);

	// Test Device ID receive
	const DeviceIdentifier testDevID = MPICommunication::recvDeviceID(comm, MPI_ANY_TAG);
	ASSERT_EQ(testDevID, TEST_MPI_COMM_DEVID);

	// Test Device receive
	const TestDevice1 compDev = TEST_MPI_COMM_DEV1;
	TestDevice1 tDev1;
	MPICommunication::recvDevice<TestDevice1, true>(comm, MPI_ANY_TAG, tDev1);
	ASSERT_EQ(compDev.getInt(), tDev1.getInt());
	ASSERT_EQ(compDev.getStr(), tDev1.getStr());

	ASSERT_EQ(boost::python::extract<int>(tDev1.getPy()).operator const int(), boost::python::extract<int>(compDev.getPy()).operator const int());

	spawnCmd.stopEngineProcess(1);
}
