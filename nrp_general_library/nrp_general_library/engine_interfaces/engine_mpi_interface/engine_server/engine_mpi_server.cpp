#include "nrp_general_library/engine_interfaces/engine_mpi_interface/engine_server/engine_mpi_server.h"

#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"

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

EngineMPIControl::EngineMPIControl(EngineMPIControlConst::COMMAND cmd, int info)
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

MPI_Comm EngineMPIServer::getComm()
{
	MPI_Comm retVal;
	if(MPI_Comm_get_parent(&retVal) != 0)
	{
		const auto errMsg = "Unable to fing MPI communicator with NRP Client";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	return retVal;
}

EngineMPIServer::EngineMPIServer(MPI_Comm comm)
    : _comm(comm)
{}

EngineMPIServer::EngineMPIServer()
    : EngineMPIServer(EngineMPIServer::getComm())
{}

void EngineMPIServer::getClientCmd(EngineMPIControl &cmd)
{
	MPICommunication::recvPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, cmd);
}

EngineInterface::RESULT EngineMPIServer::handleClientCmd(const EngineMPIControl &cmd)
{
	switch(cmd.cmd())
	{
		case EngineMPIControlConst::INITIALIZE:
			return this->initializeHandler(std::get<std::string>(cmd.info()));

		case EngineMPIControlConst::SHUTDOWN:
			return this->shutdownHandler(std::get<std::string>(cmd.info()));

		case EngineMPIControlConst::RUN_STEP:
			return this->runLoopStepHandler(std::get<float>(cmd.info()));

		case EngineMPIControlConst::GET_DEVICES:
			return this->getOutputDevicesHandler(std::get<int>(cmd.info()));

		case EngineMPIControlConst::SEND_DEVICES:
			return this->handleInputDevicesHandler(std::get<int>(cmd.info()));

		default:
			throw std::runtime_error("Unexcpected command received by EngineMPIServer");
	}
}

EngineInterface::RESULT EngineMPIServer::initializeHandler(const std::string &initData)
{
	if(this->_state != STOPPED)
	{
		const auto errMsg = "Initialize request was sent to running MPI engine. Stopping...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	const auto retVal = this->initialize(initData);
	this->_state = PAUSED;

	return retVal;
}

EngineInterface::RESULT EngineMPIServer::shutdownHandler(const std::string &shutdownData)
{
	if(this->_state == STOPPED)
	{
		const auto errMsg = "Shutdown request was sent to stopped MPI engine. Aborting...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	const auto retVal = this->shutdown(shutdownData);
	this->_state = STOPPED;

	return retVal;
}

EngineInterface::RESULT EngineMPIServer::runLoopStepHandler(float timeStep)
{
	if(this->_state != PAUSED)
	{
		const auto errMsg = "RunLoop request was sent to unpaused MPI engine. Aborting...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	this->_state = RUNNING;
	const auto retVal = this->runLoopStep(timeStep);
	this->_state = PAUSED;

	return retVal;
}

EngineInterface::RESULT EngineMPIServer::getOutputDevicesHandler(const int numDevices)
{
	if(this->_state != PAUSED)
	{
		const auto errMsg = "Get device data request was sent to unpaused MPI engine. Aborting...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

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
			const auto errMsg = "Failed to retrieve deviceID from NRP Client. Aborting...";
			std::cerr << e.what() << "\n" << errMsg << "\n";
			throw std::runtime_error(errMsg);
		}
	}

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
			const auto errMsg = "Failed to send device data \"" + devID.Name + "\" . Aborting...";
			std::cerr << e.what() << "\n" << errMsg << "\n";
			throw std::runtime_error(errMsg);
		}
	}

	return EngineInterface::SUCCESS;
}

EngineInterface::RESULT EngineMPIServer::handleInputDevicesHandler(const int numDevices)
{
	if(this->_state != PAUSED)
	{
		const auto errMsg = "Handle device data request was sent to unpaused MPI engine. Aborting...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	std::vector<DeviceIdentifier> devIDs;
	devIDs.reserve(numDevices);

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
			const auto errMsg = "Failed to retrieve device ID from NRP Client. Aborting...";
			std::cerr << e.what() << "\n" << errMsg << "\n";
			throw std::runtime_error(errMsg);
		}

		this->handleDeviceInput(devID);
	}

	return EngineInterface::SUCCESS;
}

EngineInterface::RESULT EngineMPIServer::initialize(const std::string &)
{
	return EngineInterface::SUCCESS;
}

EngineInterface::RESULT EngineMPIServer::shutdown(const std::string &)
{
	return EngineInterface::SUCCESS;
}

MPIPropertyData EngineMPIServer::getDeviceOutput(const DeviceIdentifier &devID)
{
	const auto devCtrlIt = this->_deviceControllers.find(devID.Name);
	if(devCtrlIt == this->_deviceControllers.end())
	{
		const auto errMsg = "No controller registered for device with name \"" + devID.Name + "\" . Aborting...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	try
	{
		return devCtrlIt->second->getDeviceOutput();
	}
	catch(std::exception &e)
	{
		const auto errMsg = "Failed to retrieve device data from \"" + devID.Name + "\" . Aborting...";
		std::cerr << e.what() << "\n" << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}
}

EngineInterface::RESULT EngineMPIServer::handleDeviceInput(const DeviceIdentifier &devID)
{
	const auto devCtrlIt = this->_deviceControllers.find(devID.Name);
	if(devCtrlIt == this->_deviceControllers.end())
	{
		const auto errMsg = "No controller registered for device with name \"" + devID.Name + "\" . Aborting...";
		std::cerr << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	try
	{
		devCtrlIt->second->handleMPIDeviceInput(this->_comm, EngineMPIControlConst::DEVICE_TAG);
	}
	catch(std::exception &e)
	{
		const auto errMsg = "Failed to retrieve device data from NRP Client. Aborting...";
		std::cerr << e.what() << "\n" << errMsg << "\n";
		throw std::runtime_error(errMsg);
	}

	return EngineInterface::SUCCESS;
}

//EngineInterface::device_outputs_t EngineMPIServer::getOutputDevices(const EngineInterface::device_identifiers_t &deviceIdentifiers)
//{
//	EngineMPIControl devCmd(EngineMPIControl::GET_DEVICES, deviceIdentifiers.size());
//	MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, devCmd);

//	std::vector<MPIDeviceData> mpiDeserializers;
//	mpiDeserializers.reserve((deviceIdentifiers.size()));


//	// Send device identifiers
//	for(const auto &devID : deviceIdentifiers)
//	{
//		if(devID.EngineName == this->engineName())
//		{
//			MPICommunication::sendDeviceID(this->_comm, EngineMPIControlConst::DEVICE_TAG, devID);
//			mpiDeserializers.emplace_back(MPIDeviceData(devID));
//		}
//	}

//	// Receive device data
//	EngineInterface::device_outputs_t retVal;
//	retVal.reserve(mpiDeserializers.size());

//	for(auto &deserializer : mpiDeserializers)
//	{	retVal.emplace_back(MPICommunication::recvDeviceByType<DEVICES..., false>(this->_comm, EngineMPIControlConst::DEVICE_TAG, deserializer));	}

//	return retVal;
//}

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
	else if(std::holds_alternative<int>(prop.info))
	{
		constexpr int counts[] = {1,1};
		const MPI_Aint disp[] = {getMPIAddr(&prop.cmd), getMPIAddr(&std::get<int>(prop.info))};
		const MPI_Datatype datat[] = {MPI_INT, MPI_INT};

		MPI_Type_create_struct(2, counts, disp, datat, &newType);
	}
	else
	{
		assert(std::holds_alternative<std::string>(prop.info));

		const int counts[] = {1, static_cast<int>(std::get<std::string>(prop.info).size()+1)};
		const MPI_Aint disp[] = {getMPIAddr(&prop.cmd), getMPIAddr(std::get<std::string>(prop.info).data())};
		const MPI_Datatype datat[] = {MPI_INT, MPI_CHAR};

		MPI_Type_create_struct(2, counts, disp, datat, &newType);
	}

	MPI_Type_commit(&newType);
	return std::tuple(MPIDerivedDatatype(newType), getMPIAddr(MPI_BOTTOM), 1);
}
