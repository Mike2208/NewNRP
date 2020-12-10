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

#include "nrp_grpc_engine_protocol/engine_server/engine_grpc_opts_parser.h"

cxxopts::Options EngineGRPCOptsParser::createOptionParser(bool allowUnrecognised)
{
	cxxopts::Options options("NRP Gazebo Server");

	if(allowUnrecognised)
		options.allow_unrecognised_options();

	options.add_options()
	        (EngineGRPCConfigConst::EngineServerAddrArg.data(), "NRP URL Address", cxxopts::value<std::string>()->default_value(EngineGRPCConfigConst::DefEngineServerAddress.data()))
	        (EngineGRPCConfigConst::EngineRegistrationServerAddrArg.data(), "NRP URL Registration Address", cxxopts::value<std::string>()->default_value(EngineGRPCConfigConst::DefEngineRegistrationServerAddress.data()))
	        (EngineGRPCConfigConst::EngineNameArg.data(), "NRP Engine Name", cxxopts::value<std::string>()->default_value(""));

	return options;
}

cxxopts::ParseResult EngineGRPCOptsParser::parseOpts(int argc, char *argv[], cxxopts::Options parser)
{
	return parser.parse(argc, argv);
}
