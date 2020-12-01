#ifndef ENGINE_GRPC_OPTS_PARSER_H
#define ENGINE_GRPC_OPTS_PARSER_H

#include <cxxopts.hpp>

#include "nrp_grpc_engine_protocol/config/engine_grpc_config.h"

/*!
 *  \brief Engine GRPC Executable parameter parser
 */
class EngineGRPCOptsParser
{
	public:
		static cxxopts::Options createOptionParser(bool allowUnrecognised = false);
		static cxxopts::ParseResult parseOpts(int argc, char *argv[], cxxopts::Options parser = EngineGRPCOptsParser::createOptionParser());
};

#endif // ENGINE_GRPC_OPTS_PARSER_H
