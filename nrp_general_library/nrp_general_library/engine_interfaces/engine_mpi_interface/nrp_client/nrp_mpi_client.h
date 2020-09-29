#ifndef NRP_MPI_CLIENT_H
#define NRP_MPI_CLIENT_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/engine_server/engine_mpi_server.h"

#include <future>

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
			EngineMPIControl initCmd(EngineMPIControl::INITIALIZE, this->engineConfig()->writeConfig().dump());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, initCmd);

			return EngineInterface::SUCCESS;
		}

		EngineInterface::RESULT shutdown() override
		{
			EngineMPIControl shutdownCmd(EngineMPIControl::SHUTDOWN, std::string());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, shutdownCmd);

			return EngineInterface::SUCCESS;
		}

		float getEngineTime() const override
		{	return this->_engineTime;	}

		EngineInterface::step_result_t runLoopStep(float timeStep) override
		{
			this->_runLoopThread = std::async(std::launch::async, loopThread, timeStep, this->_comm);
			return EngineInterface::SUCCESS;
		}

		EngineInterface::RESULT waitForStepCompletion(float timeOut) override
		{
			if(this->_runLoopThread.wait_for(std::chrono_literals::operator""ms(timeOut)) != std::future_status::ready)
				return EngineInterface::ERROR;

			this->_engineTime += this->_runLoopThread.get();
			return EngineInterface::SUCCESS;
		}

		EngineInterface::device_outputs_t getOutputDevices(const EngineInterface::device_identifiers_t &deviceIdentifiers) override
		{
			EngineMPIControl devCmd(EngineMPIControl::GET_DEVICES, deviceIdentifiers.size());
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
			EngineInterface::device_outputs_t retVal;
			retVal.reserve(mpiDeserializers.size());

			for(auto &deserializer : mpiDeserializers)
			{	retVal.emplace_back(MPICommunication::recvDeviceByType<DEVICES..., false>(this->_comm, EngineMPIControlConst::DEVICE_TAG, deserializer));	}

			return retVal;
		}

		EngineInterface::RESULT handleInputDevices(const EngineInterface::device_inputs_t &inputDevices) override
		{
			EngineMPIControl devCmd(EngineMPIControl::SEND_DEVICES, inputDevices.size());
			MPICommunication::sendPropertyTemplate(this->_comm, EngineMPIControlConst::GENERAL_COMM_TAG, devCmd);

			// Send device data with IDs
			for(auto *const inDev : inputDevices)
			{
				if(inDev->engineName() == this->engineName())
				{
					MPICommunication::sendDeviceByType<DEVICES..., true>(*inDev);
				}
			}

			return EngineInterface::SUCCESS;
		}

	private:
		MPI_Comm _comm;

		float _engineTime;

		std::future<float> _runLoopThread;

		static EngineInterface::step_result_t loopThread(float timestep, MPI_Comm comm)
		{
			EngineMPIControl runCmd(EngineMPIControl::RUN_STEP, timestep);
			MPICommunication::sendPropertyTemplate(comm, EngineMPIControlConst::GENERAL_COMM_TAG, runCmd);

			// Block until a response has been received, indicating the loop has completed
			MPICommunication::recvMPI(nullptr, 0, MPI_DATATYPE_NULL, MPI_ANY_SOURCE, EngineMPIControlConst::WAIT_LOOP_COMM_TAG, comm);

			return EngineInterface::SUCCESS;
		}
};

#endif // NRP_MPI_CLIENT_H
