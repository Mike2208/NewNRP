#include "nrp_json_engine_protocol/engine_server/engine_json_opts_parser.h"

cxxopts::Options EngineJSONOptsParser::createOptionParser(bool allowUnrecognised)
{
	cxxopts::Options options("NRP Gazebo Server");

	if(allowUnrecognised)
		options.allow_unrecognised_options();

	options.add_options()
	        (EngineJSONConfigConst::EngineServerAddrArg.data(), "NRP URL Address", cxxopts::value<std::string>()->default_value(EngineJSONConfigConst::DefEngineServerAddress.data()))
	        (EngineJSONConfigConst::EngineRegistrationServerAddrArg.data(), "NRP URL Registration Address", cxxopts::value<std::string>()->default_value(EngineJSONConfigConst::DefEngineRegistrationServerAddress.data()))
	        (EngineJSONConfigConst::EngineNameArg.data(), "NRP Engine Name", cxxopts::value<std::string>()->default_value(""));

	return options;
}

cxxopts::ParseResult EngineJSONOptsParser::parseOpts(int argc, char *argv[], cxxopts::Options parser)
{
	return parser.parse(argc, argv);
}
