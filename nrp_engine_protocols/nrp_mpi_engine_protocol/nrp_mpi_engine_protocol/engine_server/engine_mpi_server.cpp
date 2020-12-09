#include "nrp_general_library/engine_interfaces/engine_mpi_interface/engine_server/engine_mpi_server.h"

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"
#include "nrp_general_library/utils/mpi_setup.h"
#include "nrp_general_library/utils/nrp_logger.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <type_traits>

EngineMPIControl::EngineMPIControl(const PropertyTemplate &props)
    : PropertyTemplate(props)
{}

EngineMPIControl::EngineMPIControl(PropertyTemplate &&props)
    : PropertyTemplate(std::move(props))
{}


EngineMPIControl::EngineMPIControl(EngineMPIControlConst::CommandData cmdData)
    : PropertyTemplate(cmdData)
{}

EngineMPIControl::EngineMPIControl(EngineMPIControlConst::COMMAND cmd, const std::string &info)
    : PropertyTemplate(EngineMPIControlConst::CommandData(cmd, info))
{}

EngineMPIControl::EngineMPIControl(EngineMPIControlConst::COMMAND cmd, float info)
    : PropertyTemplate(EngineMPIControlConst::CommandData(cmd, info))
{}

EngineMPIControl::EngineMPIControl(EngineMPIControlConst::COMMAND cmd, int64_t info)
    : PropertyTemplate(EngineMPIControlConst::CommandData(cmd, info))
{}

EngineMPIControl::COMMAND &EngineMPIControl::cmd()
{
	return this->getPropertyByName<"cmd">().cmd;
}

EngineMPIControl::COMMAND EngineMPIControl::cmd() const
{
	return this->getPropertyByName<"cmd">().cmd;
}

EngineMPIControl::CommandData::info_t &EngineMPIControl::info()
{
	return this->getPropertyByName<"cmd">().info;
}

const EngineMPIControl::CommandData::info_t &EngineMPIControl::info() const
{
	return this->getPropertyByName<"cmd">().info;
}

MPI_Comm EngineMPIServer::getNRPComm()
{
	MPI_Comm retVal = MPISetup::getParentComm();
	if(retVal == MPI_COMM_NULL)
		throw NRPException::logCreate("Unable to find MPI communicator with NRP Client");

	return retVal;
}

EngineMPIServer::EngineMPIServer(MPI_Comm comm)
    : _comm(comm)
{}

EngineMPIServer::EngineMPIServer()
    : EngineMPIServer(EngineMPIServer::getNRPComm())
{}

void EngineMPIServer::getClientCmd(EngineMPIControl &cmd)
{
	MPICommunication::recvPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, cmd);
}

void EngineMPIServer::handleClientCmd(const EngineMPIControl &cmd)
{
	switch(cmd.cmd())
	{
		case EngineMPIControlConst::INITIALIZE:
			return this->initializeHandler(std::get<std::string>(cmd.info()));

		case EngineMPIControlConst::SHUTDOWN:
			return this->shutdownHandler(std::get<std::string>(cmd.info()));

		case EngineMPIControlConst::RUN_STEP:
			return this->runLoopStepHandler(SimulationTime(std::get<int64_t>(cmd.info())));

		case EngineMPIControlConst::GET_DEVICES:
			return this->requestOutputDevicesHandler(std::get<int64_t>(cmd.info()));

		case EngineMPIControlConst::SEND_DEVICES:
			return this->handleInputDevicesHandler(std::get<int64_t>(cmd.info()));

		default:
			throw NRPException::logCreate("Unexpected command received by EngineMPIServer");
	}
}

void EngineMPIServer::initializeHandler(const std::string &initData)
{
	if(this->_state != STOPPED)
		throw NRPException::logCreate("Initialize request was sent to running MPI engine. Stopping...");

	this->initialize(initData);
	this->_state = PAUSED;

	const auto retVal = true;
	MPICommunication::sendMPI(&retVal, sizeof(retVal), MPI_BYTE, 0, EngineMPIControlConst::GENERAL_COMM_TAG, this->_comm);
}

void EngineMPIServer::shutdownHandler(const std::string &shutdownData)
{
	if(this->_state == STOPPED || this->_state == STOPPING)
		throw NRPException::logCreate("Shutdown request was sent to stopped MPI engine. Aborting...");

	this->_state = STOPPING;
	this->shutdown(shutdownData);
	this->_state = STOPPED;

	const bool retVal = true;
	MPICommunication::sendMPI(&retVal, sizeof(retVal), MPI_BYTE, 0, EngineMPIControlConst::GENERAL_COMM_TAG, this->_comm);

	MPISetup::finalize();
}

void EngineMPIServer::runLoopStepHandler(SimulationTime timeStep)
{
	if(this->_state != PAUSED)
		throw NRPException::logCreate("RunLoop request was sent to unpaused MPI engine. Aborting...");

	this->_state = RUNNING;
	this->runLoopStep(timeStep);
	this->_state = PAUSED;

	int64_t engineTime = this->getSimTime().count();

	static_assert(std::is_same<decltype(engineTime), SimulationTime::rep>::value, "Mismatch between MPI type and SimulationTime underlying integer type");

	// Inform client that loop has completed
	MPICommunication::sendMPI(&engineTime, 1, MPI_INT64_T, 0, EngineMPIControlConst::WAIT_LOOP_COMM_TAG, this->_comm);
}

void EngineMPIServer::requestOutputDevicesHandler(const int numDevices)
{
	if(this->_state != PAUSED)
		throw NRPException::logCreate("Get device data request was sent to unpaused MPI engine. Aborting...");

	std::vector<DeviceIdentifier> devIDs;
	devIDs.reserve(numDevices);

	// Read device IDs
	for(int i=0; i<numDevices; ++i)
	{
		try
		{
			devIDs.emplace_back(MPICommunication::recvDeviceID(this->_comm, EngineMPIControlConst::DEVICE_TAG));
		}
		catch(std::exception &e)
		{
			throw NRPException::logCreate(e, "Failed to retrieve deviceID from NRP Client. Aborting...");
		}
	}

	EngineMPIServer::lock_t lock(this->_devCtrlLock);

	// Retrieve device data from controllers
	MPIPropertyData mpiDevDat;
	for(const auto &devID : devIDs)
	{
		mpiDevDat = this->getDeviceOutput(devID);

		try
		{
			MPICommunication::sendMPIPropertyData(this->_comm, EngineMPIControlConst::DEVICE_TAG, mpiDevDat);
		}
		catch(std::exception &e)
		{
			throw NRPException::logCreate(e, "Failed to send device data \"" + devID.Name + "\". Aborting...");
		}
	}
}

void EngineMPIServer::handleInputDevicesHandler(const int numDevices)
{
	if(this->_state != PAUSED)
		throw NRPException::logCreate("Handle device data request was sent to unpaused MPI engine. Aborting...");

	std::vector<DeviceIdentifier> devIDs;
	devIDs.reserve(numDevices);

	EngineMPIServer::lock_t lock(this->_devCtrlLock);

	// Read device data
	DeviceIdentifier devID("", "", "");
	for(int i=0; i<numDevices; ++i)
	{
		try
		{
			devID = MPICommunication::recvDeviceID(this->_comm, EngineMPIControlConst::DEVICE_TAG);
		}
		catch(std::exception &e)
		{
			throw NRPException::logCreate(e, "Failed to retrieve device ID from NRP Client. Aborting...");
		}

		this->handleDeviceInput(devID);
	}

	const bool retVal = true;
	MPICommunication::sendMPI(&retVal, sizeof(retVal), MPI_BYTE, 0, EngineMPIControlConst::GENERAL_COMM_TAG, this->_comm);
}

void EngineMPIServer::registerDeviceController(EngineMPIDeviceControllerInterface *devCtrl)
{
	EngineMPIServer::lock_t lock(this->_devCtrlLock);

	const auto devCtrlIt = this->_deviceControllers.find(devCtrl->Name);
	if(devCtrlIt != this->_deviceControllers.end())
	{
		NRPLogger::SPDWarnLogDefault("Warning: DeviceController already registered for \"" + devCtrl->Name + "\". Overriding...");
		devCtrlIt->second = devCtrl;
	}
	else
		this->_deviceControllers.emplace(devCtrl->Name, devCtrl);
}

void EngineMPIServer::removeDeviceController(const std::string &devName)
{
	EngineMPIServer::lock_t lock(this->_devCtrlLock);

	const auto devCtrlIt = this->_deviceControllers.find(devName);
	if(devCtrlIt != this->_deviceControllers.end())
		this->_deviceControllers.erase(devCtrlIt);
}

void EngineMPIServer::removeAllDeviceControllers()
{
	EngineMPIServer::lock_t lock(this->_devCtrlLock);
	this->_deviceControllers.clear();
}

EngineMPIServer::state_t EngineMPIServer::getEngineState() const
{
	return this->_state;
}

void EngineMPIServer::initialize(const std::string &)
{}

void EngineMPIServer::shutdown(const std::string &)
{}

MPIPropertyData EngineMPIServer::getDeviceOutput(const DeviceIdentifier &devID)
{
	const auto devCtrlIt = this->_deviceControllers.find(devID.Name);
	if(devCtrlIt == this->_deviceControllers.end())
		throw NRPException::logCreate("No controller registered for device with name \"" + devID.Name + "\" . Aborting...");

	try
	{
		return devCtrlIt->second->getDeviceOutput();
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Failed to retrieve device data from \"" + devID.Name + "\" . Aborting...");
	}
}

void EngineMPIServer::handleDeviceInput(const DeviceIdentifier &devID)
{
	const auto devCtrlIt = this->_deviceControllers.find(devID.Name);
	if(devCtrlIt == this->_deviceControllers.end())
		throw NRPException::logCreate("No controller registered for device with name \"" + devID.Name + "\" . Aborting...");

	try
	{
		devCtrlIt->second->handleMPIDeviceInput(this->_comm, EngineMPIControlConst::DEVICE_TAG);
	}
	catch(std::exception &e)
	{
		throw NRPException::logCreate(e, "Failed to retrieve device data from NRP Client. Aborting...");
	}
}

auto MPISinglePropertySerializer<EngineMPIControlConst::CommandData>::derivedMPIDatatype(EngineMPIControlConst::CommandData &prop) -> mpi_prop_datatype_t<MPISinglePropertySerializerGeneral::MPIDerivedDatatype>
{
	MPI_Datatype newType;

	if(std::holds_alternative<float>(prop.info))
	{
		constexpr int counts[] = {1,1};
		const MPI_Aint disp[] = {getMPIAddr(&prop.cmd), getMPIAddr(&std::get<float>(prop.info))};
		const MPI_Datatype datat[] = {MPI_INT, MPI_FLOAT};

		MPI_Type_create_struct(2, counts, disp, datat, &newType);
	}
	else if(std::holds_alternative<int64_t>(prop.info))
	{
		constexpr int counts[] = {1,1};
		const MPI_Aint disp[] = {getMPIAddr(&prop.cmd), getMPIAddr(&std::get<int64_t>(prop.info))};
		const MPI_Datatype datat[] = {MPI_INT, MPI_INT64_T};

		MPI_Type_create_struct(2, counts, disp, datat, &newType);
	}
	else
	{
		assert(std::holds_alternative<std::string>(prop.info));

		std::string &info = std::get<std::string>(prop.info);
		const int counts[] = {1, static_cast<int>(std::get<std::string>(prop.info).size()+1)};
		const MPI_Aint disp[] = {getMPIAddr(&prop.cmd), getMPIAddr(info.data())};
		const MPI_Datatype datat[] = {MPI_INT, MPI_CHAR};

		MPI_Type_create_struct(2, counts, disp, datat, &newType);
	}

	MPI_Type_commit(&newType);
	return std::tuple(MPIDerivedDatatype(newType), getMPIAddr(MPI_BOTTOM), 1);
}
