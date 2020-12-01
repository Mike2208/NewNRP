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
