#ifndef NRP_MPI_CLIENT_H
#define NRP_MPI_CLIENT_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/engine_server/engine_mpi_server.h"

#include <future>
#include <type_traits>

/*!
 * \brief NRP Client for MPI Communication
 * \tparam CLIENT Final derived class of NRPMPIClient
 * \tparam CONFIG Config type corresponding to this client/engine
 * \tparam DEVICES Device types that this client/engine supports
 */
template<class CLIENT, class CONFIG, DEVICE_C ...DEVICES>
class NRPMPIClient
        : public Engine<CLIENT, CONFIG>
{
		using dcm_t = MPIDeviceConversionMechanism<DEVICES...>;

	public:
		NRPMPIClient(EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher)
		    : Engine<CLIENT, CONFIG>(config, std::move(launcher))
		{}

		EngineInterface::RESULT initialize() override
		{
			this->_comm = this->getComm();

			EngineMPIControl initCmd(EngineMPIControl::INITIALIZE, this->engineConfig()->writeConfig().dump());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, initCmd);

			EngineInterface::RESULT res;
			MPICommunication::recvMPI(&res, sizeof(res), MPI_BYTE, MPI_ANY_SOURCE, EngineMPIControlConst::GENERAL_COMM_TAG, this->_comm);

			return res;
		}

		EngineInterface::RESULT shutdown() override
		{
			EngineMPIControl shutdownCmd(EngineMPIControl::SHUTDOWN, std::string());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, shutdownCmd);

			EngineInterface::RESULT res;
			MPICommunication::recvMPI(&res, sizeof(res), MPI_BYTE, MPI_ANY_SOURCE, EngineMPIControlConst::GENERAL_COMM_TAG, this->_comm);

			return res;
		}

		SimulationTime getEngineTime() const override
		{	return this->_engineTime;	}

		EngineInterface::step_result_t runLoopStep(SimulationTime timeStep) override
		{
			this->_runLoopThread = std::async(std::launch::async, loopThread, timeStep, this->_comm);
			return EngineInterface::SUCCESS;
		}

		EngineInterface::RESULT waitForStepCompletion(float timeOut) override
		{
			if(this->_runLoopThread.wait_for(std::chrono_literals::operator""s(timeOut)) != std::future_status::ready)
				return EngineInterface::ERROR;

			const auto retEngineTime = this->_runLoopThread.get();
			if(retEngineTime < SimulationTime::zero())
				return EngineInterface::ERROR;

			this->_engineTime += retEngineTime;
			return EngineInterface::SUCCESS;
		}

		EngineInterface::RESULT handleInputDevices(const EngineInterface::device_inputs_t &inputDevices) override
		{
			EngineMPIControl devCmd(EngineMPIControl::SEND_DEVICES, (int64_t)inputDevices.size());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, devCmd);

			// Send device data with IDs
			for(auto *const inDev : inputDevices)
			{
				if(inDev->engineName() == this->engineName())
				{
					MPICommunication::template sendDeviceByType<true, DEVICES...>(this->_comm, EngineMPIControlConst::DEVICE_TAG, *inDev);
				}
			}

			EngineInterface::RESULT res;
			MPICommunication::recvMPI(&res, sizeof(res), MPI_BYTE, MPI_ANY_SOURCE, EngineMPIControlConst::GENERAL_COMM_TAG, this->_comm);

			return res;
		}

	protected:
		/*!
		 * \brief Communicator with Engine
		 */
		MPI_Comm _comm = MPI_COMM_NULL;

		/*!
		 * \brief Extract communicator from LaunchCommand
		 */
		MPI_Comm getComm() const
		{
			if(this->_process->launchCommand()->launchType() != MPISpawn::LaunchType.data())
			{
				throw NRPException::logCreate("Gazebo engine \"" + this->engineConfigGeneral()->engineName() +
				                              "\" was not created by an MPISpawn. Unable to get MPI Intercomm");
			}

			return dynamic_cast<const MPISpawn*>(this->_process->launchCommand())->getIntercomm();
		}

		EngineInterface::device_outputs_set_t requestOutputDeviceCallback(const EngineInterface::device_identifiers_t &deviceIdentifiers) override
		{
			EngineMPIControl devCmd(EngineMPIControl::GET_DEVICES, (int64_t)deviceIdentifiers.size());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, devCmd);

			std::vector<MPIDeviceData> mpiDeserializers;
			mpiDeserializers.reserve(deviceIdentifiers.size());


			// Send device identifiers
			for(const auto &devID : deviceIdentifiers)
			{
				if(devID.EngineName == this->engineName())
				{
					MPICommunication::sendDeviceID(this->_comm, EngineMPIControlConst::DEVICE_TAG, devID);
					mpiDeserializers.emplace_back(MPIDeviceData(devID));
				}
			}

			// Receive device data
			EngineInterface::device_outputs_set_t retVal;
			for(auto &deserializer : mpiDeserializers)
			{
				retVal.emplace(MPICommunication::template recvDeviceByType<false, DEVICES...>(this->_comm, (int)EngineMPIControlConst::DEVICE_TAG, deserializer));
			}

			return retVal;
		}


	private:
		/*!
		 * \brief Engine Simulation Time
		 */
		SimulationTime _engineTime;

		/*!
		 * \brief Simulation run thread. Waits for completion message from Engine
		 */
		std::future<SimulationTime> _runLoopThread;

		/*!
		 * \brief Sends simulation run command to engine.
		 * \param timestep Timestep to run engine for
		 * \param comm Engine MPI Communicator
		 * \return Returns engine time after loop completion, or negative value on error
		 */
		static SimulationTime loopThread(SimulationTime timestep, MPI_Comm comm)
		{
			EngineMPIControl runCmd(EngineMPIControl::RUN_STEP, timestep.count());
			MPICommunication::sendPropertyTemplate(comm, EngineMPIControlConst::GENERAL_COMM_TAG, runCmd);

			// Block until a response has been received, indicating the loop has completed
			
			int64_t engineTime;

			static_assert(std::is_same<decltype(engineTime), decltype(SimulationTime::count())>::value, "Mismatch between MPI type and SimulationTime underlying integer type");

			MPICommunication::recvMPI(&engineTime, 1, MPI_INT64_T, MPI_ANY_SOURCE, EngineMPIControlConst::WAIT_LOOP_COMM_TAG, comm);

			return SimulationTime(engineTime);
		}
};

#endif // NRP_MPI_CLIENT_H
