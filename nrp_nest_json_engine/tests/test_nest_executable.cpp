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

#include "nest_server_executable/nest_server_executable.h"
#include "nrp_general_library/utils/spdlog_setup.h"
#include "nrp_general_library/utils/pipe_communication.h"
#include "nrp_nest_json_engine/config/nest_config.h"
#include "tests/test_env_cmake.h"

#include <future>
#include <nlohmann/json.hpp>
#include <restclient-cpp/restclient.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>

using namespace testing;

static PipeCommunication *pCommPtC = nullptr;
static PipeCommunication *pCommCtP = nullptr;

void handle_signal(int signal)
{
	std::cout << "Handling signal";
	std::cout.flush();

	if(signal == SIGHUP)
	{
		// Shutdown server
		NestServerExecutable::shutdown();

		if(pCommPtC)
			pCommPtC->~PipeCommunication();

		if(pCommCtP)
			pCommCtP->~PipeCommunication();

		// Finish printing
		std::cout.flush();
		std::cerr.flush();
		SPDLogSetup::shutdownDefault();

		exit(0);
	}
}

TEST(TestNestExecutable, TestNest)
{	
	auto cfg = NestConfig(nlohmann::json());
	cfg.nestInitFileName() = TEST_SIMPLE_NEST_FILE_NAME;
	cfg.engineServerAddress() = "localhost:5432";

	const auto serverAddr = cfg.engineServerAddress();

	std::vector<const char*> argv;
	const std::string processID = "TestProcess";
	const std::string engineAddrArg = std::string("--") + NestConfig::EngineServerAddrArg.data();
	argv.push_back(processID.data());
	argv.push_back(engineAddrArg.data());
	argv.push_back(serverAddr.data());
	int argc = static_cast<int>(argv.size());

	SimulationTime timeStep = toSimulationTime<int, std::milli>(1);

	// Create pipe
	PipeCommunication commCtP, commPtC;
	pCommCtP = &commCtP;
	pCommPtC = &commPtC;


	// Create child process
	const auto parentPID = getpid();
	auto pid = fork();
	ASSERT_GE(pid, 0);
	if(pid > 0)
	{		
		// Parent process, main checking process
		pCommCtP->closeWrite();
		pCommPtC->closeRead();

		char rec = 0;

		// Wait for server to startup
		ASSERT_EQ(commCtP.readP(&rec, 1, 5, 1), 1);
		ASSERT_EQ(rec, 1);

		sleep(2);

		// Send init call
		RestClient::Response resp = RestClient::post(serverAddr + "/" + EngineJSONConfigConst::EngineServerInitializeRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), nlohmann::json({{NestConfig::ConfigType.m_data, cfg.writeConfig()}}).dump());
		ASSERT_EQ(resp.code, 200);

		ASSERT_EQ(commCtP.readP(&rec, 1, 5, 1), 1);
		ASSERT_EQ(rec, 2);

		// Send runstep call
		resp = RestClient::post(serverAddr + "/" + EngineJSONConfigConst::EngineServerRunLoopStepRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), nlohmann::json({{NestConfig::EngineTimeStepName.data(), timeStep.count()}}).dump());
		auto respParse = nlohmann::json::parse(resp.body);
		ASSERT_EQ(respParse[NestConfig::EngineTimeName.data()].get<float>(), timeStep.count());

		char send = 3;
		ASSERT_EQ(commPtC.writeP(&send, 1, 5, 1), 1);

		// Test shutdown
		resp = RestClient::post(serverAddr + "/" + EngineJSONConfigConst::EngineServerShutdownRoute.data(), EngineJSONConfigConst::EngineServerContentType.data(), nlohmann::json().dump());

		ASSERT_EQ(commCtP.readP(&rec, 1, 5, 1), 1);
		ASSERT_EQ(rec, 4);
	}
	else
	{
		try
		{
			// Child process, NEST executable
			ASSERT_NE(signal(SIGHUP, &handle_signal), SIG_ERR);

			// Child process should quit on parent process death
			prctl(PR_SET_PDEATHSIG, SIGHUP);

			// Stop if parent process died before setting up death handling
			if(getppid() != parentPID)
				exit(0);

			pCommPtC->closeWrite();
			pCommCtP->closeRead();

			// Start server
			auto &nestExec = NestServerExecutable::resetInstance(argc, const_cast<char**>(argv.data()));

			ASSERT_EQ(nestExec.serverRunning(), true);

			sleep(1);

			char send = 1;
			ASSERT_EQ(commCtP.writeP(&send, 1, 5, 1), 1);

			// Wait for init call
			nestExec.pyState().allowThreads();
			nestExec.waitForInit();

			send = 2;
			ASSERT_EQ(commCtP.writeP(&send, 1, 5, 1), 1);

			// Wait until runStep call has been sent
			char rec = 0;
			ASSERT_EQ(commPtC.readP(&rec, 1, 5, 1), 1);
			ASSERT_EQ(rec, 3);

			// Continue running until shutdown command has been received
			nestExec.run();

			ASSERT_EQ(nestExec.serverRunning(), false);
			send = 4;
			ASSERT_EQ(commCtP.writeP(&send, 1, 5, 1), 1);
		}
		catch(std::exception&)
		{}

		// Stop nest server
		NestServerExecutable::shutdown();

		// Shutdown pipe
		commCtP.~PipeCommunication();
		commPtC.~PipeCommunication();

		// Finish printing
		std::cout.flush();
		std::cerr.flush();
		SPDLogSetup::shutdownDefault();

		exit(0);
	}
}
