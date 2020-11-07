#include "nrp_communication_controller/nrp_communication_plugin.h"

#include "nrp_communication_controller/nrp_communication_controller.h"
#include "nrp_general_library/engine_interfaces/engine_json_interface/engine_server/engine_json_opts_parser.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

void gazebo::NRPCommunicationPlugin::Load(int argc, char **argv)
{
	std::cout << "NRP Communication plugin: Initializing...\n";

	std::string serverAddr, engineName, registrationAddr;
	try
	{
		// Parse options from input
		auto inputArgsParse = EngineJSONOptsParser::parseOpts(argc, argv, EngineJSONOptsParser::createOptionParser(true));

		// Save given URL
		serverAddr = inputArgsParse[EngineJSONConfigConst::EngineServerAddrArg.data()].as<std::string>();
		engineName = inputArgsParse[EngineJSONConfigConst::EngineNameArg.data()].as<std::string>();
		registrationAddr = inputArgsParse[EngineJSONConfigConst::EngineRegistrationServerAddrArg.data()].as<std::string>();
	}
	catch(cxxopts::OptionException &e)
	{
		throw NRPException::logCreate(std::string("Failed to parse options: \n") + e.what());
	}

	// Create server with given URL
	auto &newController = NRPCommunicationController::resetInstance(serverAddr, engineName, registrationAddr);

	// Save bound URL
	this->_serverAddress = newController.serverAddress();
	std::cout << "NRP Communication plugin: Starting server on \"" + this->_serverAddress + "\"\n";

	// Start the server
	newController.startServerAsync();

	std::cout << "NRP Communication plugin: Server started. Waiting for input...\n";
}

void gazebo::NRPCommunicationPlugin::Reset()
{
	// Reset server
	std::cout << "NRP Communication plugin: Resetting controller...\n";
	auto &newController = NRPCommunicationController::resetInstance(this->_serverAddress);

	// Start server
	newController.startServerAsync();

	std::cout << "NRP Communication plugin: Server restarted. Waiting for input...\n";
}
