#include "nrp_general_library/config/engine_config.h"

const EngineConfigConst::string_vector_t EngineConfigConst::DefEngineProcEnvParams = {};
const EngineConfigConst::string_vector_t EngineConfigConst::DefEngineProcStartParams = {};

std::string &EngineConfigGeneral::engineName()
{	return const_cast<std::string&>(const_cast<const EngineConfigGeneral*>(this)->engineName());	}

std::string &EngineConfigGeneral::engineLaunchCmd()
{	return const_cast<std::string&>(const_cast<const EngineConfigGeneral*>(this)->engineLaunchCmd());	}

float &EngineConfigGeneral::engineTimestep()
{	return const_cast<float&>(const_cast<const EngineConfigGeneral*>(this)->engineTimestep());	}

float &EngineConfigGeneral::engineCommandTimeout()
{	return const_cast<float&>(const_cast<const EngineConfigGeneral*>(this)->engineCommandTimeout());	}

EngineConfigConst::string_vector_t &EngineConfigGeneral::userProcEnvParams()
{	return const_cast<string_vector_t&>(const_cast<const EngineConfigGeneral*>(this)->userProcEnvParams());	}

std::string &EngineConfigGeneral::engineProcCmd()
{	return const_cast<std::string&>(const_cast<const EngineConfigGeneral*>(this)->engineProcCmd());	}

EngineConfigConst::string_vector_t &EngineConfigGeneral::userProcStartParams()
{	return const_cast<string_vector_t&>(const_cast<const EngineConfigGeneral*>(this)->userProcStartParams());	}

EngineConfigConst::string_vector_t EngineConfigGeneral::allEngineProcEnvParams() const
{	return this->userProcEnvParams();		}

EngineConfigConst::string_vector_t EngineConfigGeneral::allEngineProcStartParams() const
{	return this->userProcStartParams();	}
