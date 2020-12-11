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
