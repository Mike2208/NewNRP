#include "nrp_general_library/engine_interfaces/engine_interface.h"

EngineInterface::EngineInterface(std::unique_ptr<ProcessLauncherInterface> &&launcher)
    : _process(std::move(launcher))
{}

EngineInterface::~EngineInterface() = default;

const std::string &EngineInterface::engineName() const
{
	return this->engineConfigGeneral()->engineName();
}

std::string &EngineInterface::engineName()
{
	return this->engineConfigGeneral()->engineName();
}

pid_t EngineInterface::launchEngine()
{
	// Launch engine
	return this->_process->launchEngineProcess(*this->engineConfigGeneral(), EngineConfigConst::string_vector_t(), EngineConfigConst::string_vector_t());
}

EngineLauncherInterface::EngineLauncherInterface(const EngineLauncherInterface::engine_type_t &engineType)
    : _engineType(engineType)
{}

const EngineLauncherInterface::engine_type_t &EngineLauncherInterface::engineType() const
{
	return this->_engineType;
}
