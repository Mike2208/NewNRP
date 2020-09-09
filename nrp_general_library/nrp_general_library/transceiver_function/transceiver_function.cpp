#include "nrp_general_library/transceiver_function/transceiver_function.h"

TransceiverFunction::TransceiverFunction(std::string linkedEngine)
    : _linkedEngine(linkedEngine)
{}

const std::string &TransceiverFunction::linkedEngineName() const
{	return this->_linkedEngine;	}

TransceiverDeviceInterface::shared_ptr TransceiverFunction::pySetup(boost::python::object transceiverFunction)
{
	this->_function = transceiverFunction;

	auto tf = this->moveToSharedPtr<TransceiverFunction>();

	assert(tf->_tfInterpreterRegistryPtr == nullptr);
	if(tf->_tfInterpreterRegistryPtr == nullptr)
		tf->_tfInterpreterRegistryPtr = TransceiverFunction::TFInterpreter->registerNewTF(this->linkedEngineName(), tf);

	return tf;
}

boost::python::object TransceiverFunction::runTf(boost::python::tuple &args, boost::python::dict &kwargs)
{
	return this->_function(*args, **kwargs);
}

EngineInterface::device_identifiers_t TransceiverFunction::getRequestedDeviceIDs() const
{
	return EngineInterface::device_identifiers_t();
}

EngineInterface::device_identifiers_t TransceiverFunction::updateRequestedDeviceIDs(EngineInterface::device_identifiers_t &&deviceIDs) const
{
	return std::move(deviceIDs);
}

TransceiverDeviceInterface::shared_ptr *TransceiverFunction::getTFInterpreterRegistry()
{
	return this->_tfInterpreterRegistryPtr;
}
