#ifndef ENGINE_JSON_OPTS_PARSER_H
#define ENGINE_JSON_OPTS_PARSER_H

#include <cxxopts.hpp>

#include "nrp_json_engine_protocol/config/engine_json_config.h"

/*!
 *  \brief Engine JSON Executable parameter parser
 */
class EngineJSONOptsParser
{
	public:
		static cxxopts::Options createOptionParser(bool allowUnrecognised = false);
		static cxxopts::ParseResult parseOpts(int argc, char *argv[], cxxopts::Options parser = EngineJSONOptsParser::createOptionParser());
};

#endif
