#include "nrp_general_library/transceiver_function/transceiver_device_interface.h"

TransceiverFunctionInterpreter *TransceiverDeviceInterface::TFInterpreter = nullptr;

const std::string &TransceiverDeviceInterface::linkedEngineName() const
{
	return this->_function->linkedEngineName();
}

boost::python::object TransceiverDeviceInterface::runTf(boost::python::tuple &args, boost::python::dict &kwargs)
{
	return this->_function->runTf(args, kwargs);
}

EngineInterface::device_identifiers_t TransceiverDeviceInterface::updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs) const
{
	auto subDeviceIDs = this->_function->updateRequestedDeviceIDs(std::move(deviceIDs));
	auto newDeviceIDs = this->getRequestedDeviceIDs();

	subDeviceIDs.insert(newDeviceIDs.begin(), newDeviceIDs.end());

	return subDeviceIDs;
}

EngineInterface::device_identifiers_t TransceiverDeviceInterface::getRequestedDeviceIDs() const
{
	return EngineInterface::device_identifiers_t();
}

void TransceiverDeviceInterface::setTFInterpreter(TransceiverFunctionInterpreter *interpreter)
{
	TransceiverDeviceInterface::TFInterpreter = interpreter;
}

TransceiverDeviceInterface::shared_ptr *TransceiverDeviceInterface::getTFInterpreterRegistry()
{
	return this->_function->getTFInterpreterRegistry();
}
