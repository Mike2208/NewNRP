#include "nrp_nest_server_engine/config/nest_server_config.h"

#include "nrp_general_library/config/cmake_constants.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const size_t NestServerConfigConst::DefNestRNGSeed = static_cast<size_t>(time(nullptr));

uint16_t NestServerConfig::NextDefPort = PortSearchStart;

uint16_t findUnboundPort(uint16_t &startPort)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
		throw std::runtime_error(std::string("Socket Error: ") + strerror(errno));

	struct sockaddr_in serv_addr;
	do
	{
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(startPort);
		if(bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0)
			break;

		if(errno != EADDRINUSE)
			throw std::runtime_error("Failed to bind port " + std::to_string(startPort) + ": " + strerror(errno));

		startPort += 1;
	}
	while(true);

	if(close(sock) < 0)
		throw std::runtime_error("Failed to close socket at port " + std::to_string(startPort) + ": " + strerror(errno));

	return startPort++;
}

NestServerConfig::NestServerConfig(EngineConfigConst::config_storage_t &config)
    : NestServerConfig(config.Data)
{
	config.Config = this;
}

NestServerConfig::NestServerConfig(const nlohmann::json &data)
    : EngineJSONConfig(data,
                       NestServerConfig::DefNestRNGSeed, NestServerConfig::DefNestInitFileName.data(),
                       NestServerConfig::DefNestServerHost.data(), findUnboundPort(NextDefPort))
{}

size_t NestServerConfig::nestRNGSeed() const
{
	return this->getPropertyByName<NestServerConfig::NestRNGSeed>();
}

size_t &NestServerConfig::nestRNGSeed()
{
	return this->getPropertyByName<NestServerConfig::NestRNGSeed>();
}

std::string &NestServerConfig::nestInitFileName()
{
	return this->getPropertyByName<NestServerConfig::NestInitFileName>();
}

const std::string &NestServerConfig::nestInitFileName() const
{
	return this->getPropertyByName<NestServerConfig::NestInitFileName>();
}

const std::string &NestServerConfig::nestServerHost() const
{
	return this->getPropertyByName<NestServerConfig::NestServerHost>();
}

std::string &NestServerConfig::nestServerHost()
{
	return this->getPropertyByName<NestServerConfig::NestServerHost>();
}

const uint16_t &NestServerConfig::nestServerPort() const
{
	return this->getPropertyByName<NestServerConfig::NestServerPort>();
}

uint16_t &NestServerConfig::nestServerPort()
{
	return this->getPropertyByName<NestServerConfig::NestServerPort>();
}

EngineConfigConst::string_vector_t NestServerConfig::allEngineProcEnvParams() const
{
	EngineConfigConst::string_vector_t envVars = this->EngineJSONConfig::allEngineProcEnvParams();;

	// Add NRP library path
	envVars.push_back("LD_LIBRARY_PATH=" NRP_LIB_INSTALL_DIR ":$LD_LIBRARY_PATH");

	// Disable Nest output
	envVars.push_back("PYNEST_QUIET=1");

	// Disable RestrictedPython
	envVars.push_back("NEST_SERVER_RESTRICTION_OFF=1");

	return envVars;
}

EngineConfigConst::string_vector_t NestServerConfig::allEngineProcStartParams() const
{
	EngineConfigConst::string_vector_t startParams;

	// Add Server address
	startParams.push_back("-o");
	startParams.push_back("-h"); startParams.push_back(this->nestServerHost());
	startParams.push_back("-p"); startParams.push_back(std::to_string(this->nestServerPort()));

	return startParams;
}
