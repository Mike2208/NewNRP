#include "nrp_server/experiment_manager.h"

#include "nrp_general_library/utils/zip_container.h"
#include "nrp_simulation/server/simulation_server.h"

ExperimentManager::ExperimentManager(const ServerConfigConstSharedPtr &config)
    : _config(config),
      _router(ExperimentManager::setupServerRoutes(this)),
      _server(Pistache::Address(config->serverAddress()))
{
	this->_server.setHandler(this->_router.handler());
	this->startServerAsync();
}

ExperimentManager::~ExperimentManager()
{
	this->shutdownServer();
}

void ExperimentManager::startServerAsync()
{
	if(!this->_serverRunning)
	{
		this->_server.serveThreaded();
		this->_serverRunning = true;
	}
}

void ExperimentManager::shutdownServer()
{
	if(this->_serverRunning)
	{
		this->_server.shutdown();
		this->_serverRunning = false;
	}
}

bool ExperimentManager::isServerRunning() const
{
	return this->_serverRunning;
}

void ExperimentManager::shutdownExperiment(const exp_map_t::iterator &expIt, const lock_t &expLock)
{
	PipeCommPacket pack;
	pack.ID = -1;
	pack.Command = SimulationServerConst::ShutdownCommand.data();

	const auto packID = expIt->second.PComm.sendPacket(std::move(pack));


}

Pistache::Rest::Router ExperimentManager::setupServerRoutes(ExperimentManager *expManager)
{
	Pistache::Rest::Router router;

	Pistache::Rest::Routes::Post(router, ExperimentManager::GetRunningExperimentsRoute.data(), Pistache::Rest::Routes::bind(&ExperimentManager::getRunningExperimentsHandler, expManager));
	Pistache::Rest::Routes::Post(router, ExperimentManager::GetServerStatusRoute.data(), Pistache::Rest::Routes::bind(&ExperimentManager::getServerStatusHandler, expManager));
	Pistache::Rest::Routes::Post(router, ExperimentManager::UploadExperimentRoute.data(), Pistache::Rest::Routes::bind(&ExperimentManager::uploadExperimentHandler, expManager));
	Pistache::Rest::Routes::Post(router, ExperimentManager::StartExperimentRoute.data(), Pistache::Rest::Routes::bind(&ExperimentManager::startExperimentHandler, expManager));

	return router;
}

void ExperimentManager::uploadExperimentHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res)
{
	try
	{
		const std::string expName  = ExperimentManager::getExperimentName(req);
		const std::string userName = ExperimentManager::getUsername(req);

		ZipContainer zipDat(req.body());

		const auto expPath = this->_config->serverExperimentDirectory() / userName / expName;
		try
		{
			std::filesystem::create_directories(expPath);
		}
		catch(std::exception &e)
		{
			const auto errMsg = std::string("Failed to create experiment directories: ") + e.what();

			std::cerr << errMsg << std::endl;
			throw;
		}

		zipDat.extractZipFiles(expPath);
	}
	catch(std::exception &e)
	{
		res.send(Pistache::Http::Code::Internal_Server_Error, e.what());
		return;
	}

	res.send(Pistache::Http::Code::Ok);
}

void ExperimentManager::startExperimentHandler(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter res)
{

}

std::string ExperimentManager::getUsername(const Pistache::Rest::Request &req)
{
	std::string userName = ServerConfigConst::DefaultUsername.data();
	if(const auto authPtr = req.headers().get("Authorization"))
		userName = dynamic_cast<const Pistache::Http::Header::Authorization*>(authPtr.get())->getBasicUser();

	return userName;
}

std::string ExperimentManager::getExperimentName(const Pistache::Rest::Request &req)
{
	std::string expName;
	try
	{	auto expName = req.param(UploadExperimentNameParam.data()).as<std::string>();	}
	catch(std::exception&)
	{	expName = ServerConfigConst::DefaultExperimentName.data();	}

	return expName;
}
