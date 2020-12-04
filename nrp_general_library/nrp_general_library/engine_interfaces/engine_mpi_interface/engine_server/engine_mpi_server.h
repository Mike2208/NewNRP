#ifndef ENGINE_MPI_SERVER_H
#define ENGINE_MPI_SERVER_H

#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/config/engine_mpi_config.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/engine_server/engine_mpi_device_controller.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

#include <mutex>
#include <variant>

struct EngineMPIControlConst
{
	enum MPI_TAG
	{	DEVICE_TAG = 0, GENERAL_COMM_TAG, WAIT_LOOP_COMM_TAG	};

	enum COMMAND
	{	SHUTDOWN, INITIALIZE, RUN_STEP, GET_DEVICES, SEND_DEVICES, NONE	};


	struct CommandData
	{
		using info_t = std::variant<float, int64_t, std::string>;

		COMMAND cmd = NONE;
		info_t info = 0;

		CommandData() = default;

		CommandData(COMMAND _cmd, const std::string &_info)
		    : cmd(_cmd),
		      info(_info)
		{}

		CommandData(COMMAND _cmd, float _info)
		    : cmd(_cmd),
		      info(_info)
		{}

		CommandData(COMMAND _cmd, int64_t _info)
		    : cmd(_cmd),
		      info(_info)
		{}
	};
};

/*!
 * \brief Control data for an MPI server. Clients can use this to send commands to a server
 */
class EngineMPIControl
        : public PropertyTemplate<EngineMPIControl, PropNames<"cmd">, EngineMPIControlConst::CommandData>,
          public EngineMPIControlConst
{
	public:
		EngineMPIControl();

		EngineMPIControl(const PropertyTemplate &props);
		EngineMPIControl(PropertyTemplate &&props);

		EngineMPIControl(EngineMPIControlConst::CommandData cmdData);
		EngineMPIControl(EngineMPIControlConst::COMMAND cmd, const std::string &info);
		EngineMPIControl(EngineMPIControlConst::COMMAND cmd, float info);
		EngineMPIControl(EngineMPIControlConst::COMMAND cmd, int64_t info);

		/*!
		 * \brief Get command
		 */
		COMMAND &cmd();

		/*!
		 * \brief Get command
		 */
		COMMAND cmd() const;

		/*!
		 * \brief Get command info
		 */
		CommandData::info_t &info();

		/*!
		 * \brief Get command info
		 */
		const CommandData::info_t &info() const;
};

class EngineMPIServer
{
	public:
		/*!
		 * \brief Server states. Will be set accordingly
		 * - STOPPED:     Server state at startup and after server has shut down
		 * - PAUSED:      State after initialize has been called and sim is not running
		 * - RUNNING:     State during runLoopStep. After step has completed, state should be set back to PAUSED
		 * - STOPPING:    State during shutdown call
		 */
		enum state_t
		{
			STOPPED,
			PAUSED,
			RUNNING,
			STOPPING
		};

		/*!
		 * \brief Get Communicator to NRP Client, assuming that this engine was spawned as a child process of the CLE.
		 * Uses MPI_Comm_get_parent()
		 * \exception Throws std::runtime_error if no communicator with parent can be found
		 */
		static MPI_Comm getNRPComm();

		/*!
		 * \brief Constructor
		 * \param comm MPI communicator used for PTP communication with NRP client
		 */
		EngineMPIServer(MPI_Comm comm);

		/*!
		 * \brief Constructor.
		 * Will initialize _comm with MPI_Comm_get_parent()
		 */
		EngineMPIServer();

		// Delete copy operations, server can only be moved
		EngineMPIServer(const EngineMPIServer&) = delete;
		EngineMPIServer(EngineMPIServer&&) = default;

		EngineMPIServer &operator=(const EngineMPIServer&) = delete;
		EngineMPIServer &operator=(EngineMPIServer&&) = default;

		/*!
		 * \brief Get command from client. Blocks until command was received
		 * \param cmd Command output
		 */
		void getClientCmd(EngineMPIControl &cmd);

		/*!
		 * \brief Handle a received client command
		 * \param cmd Command to process
		 * \return Returns result of command processing
		 */
		EngineInterface::RESULT handleClientCmd(const EngineMPIControl &cmd);

		/*!
		 * \brief Calls initialize() and sets _state
		 * \param initData Initialization data
		 * \return Returns result of initialize()
		 * \exception Throws std::runtime_error if sim was not in STOPPED state at call time,
		 * or any exception initialize() generates
		 */
		EngineInterface::RESULT initializeHandler(const std::string &initData);

		/*!
		 * \brief Calls shutdown() and sets _state
		 * \param shutdownData Shutdown data
		 * \return Returns result of shutdown()
		 * \exception Throws std::runtime_error if sim was not in PAUSED state at call time,
		 * or any exception shutdown() generates
		 */
		EngineInterface::RESULT shutdownHandler(const std::string &shutdownData);

		/*!
		 * \brief Calls runLoopSte() and sets _state
		 * \param timeStep Timestep to run the sim
		 * \return Returns result of runLoopStep()
		 * \exception Throws std::runtime_error if sim was not in PAUSED state at call time,
		 * or any exception runLoopStep() generates
		 */
		EngineInterface::RESULT runLoopStepHandler(SimulationTime timeStep);

		/*!
		 * \brief Calls requestOutputDevices() and sets _state
		 * \param numDevices Number of devices that should be sent
		 * \return Returns result of requestOutputDevices()
		 * \exception Throws std::runtime_error if sim was not in PAUSED state at call time,
		 * or any exception requestOutputDevices() generates
		 */
		EngineInterface::RESULT requestOutputDevicesHandler(const int numDevices);

		/*!
		 * \brief Calls handleInputDevices() and sets _state
		 * \param numDevices Number of devices that should be sent
		 * \return Returns result of handleInputDevices()
		 * \exception Throws std::runtime_error if sim was not in PAUSED state at call time,
		 * or any exception handleInputDevices() generates
		 */
		EngineInterface::RESULT handleInputDevicesHandler(const int numDevices);

		/*!
		 * \brief Register a new device controller for incoming/outgoing device data
		 * \param devCtrl Device Controller to register
		 * \return Returns result of operation
		 */
		EngineInterface::RESULT registerDeviceController(EngineMPIDeviceControllerInterface *devCtrl);

		/*!
		 * \brief Remove an already registered device controller
		 * \param devName Name of device whose DeviceController should be removed
		 * \return Returns result of operation
		 */
		EngineInterface::RESULT removeDeviceController(const std::string &devName);

		/*!
		 * \brief Remove all device controllers
		 */
		void removeAllDeviceControllers();

		/*!
		 * \brief Get current engine state
		 */
		state_t getEngineState() const;

	protected:
		/*!
		 * \brief User-defined initialization function
		 * \param initData Initialization data sent from the CLE
		 * \return Returns result of initialization
		 */
		virtual EngineInterface::RESULT initialize(const std::string &initData);

		/*!
		 * \brief User-defined shutdown function
		 * \param initData Shutdown data sent from the CLE
		 * \return Returns result of shutdown
		 */
		virtual EngineInterface::RESULT shutdown(const std::string &shutdownData);

		/*!
		 * \brief User-defined simulation run function
		 * \param timeStep Time (in s) to run the sim
		 * \return Returns result of running the sim
		 */
		virtual EngineInterface::step_result_t runLoopStep(SimulationTime timeStep) = 0;

		/*!
		 * \brief User-defined fcn to get the total runtime of the sim
		 */
		virtual SimulationTime getSimTime() const = 0;

		/*!
		 * \brief User-defined fcn to get device output data. Can be overriden
		 * \param devID ID of device to retrieve
		 * \return Returns data to be sent
		 */
		virtual MPIPropertyData getDeviceOutput(const DeviceIdentifier &devID);

		/*!
		 * \brief User-defined fcn to handle device input data. Can be overriden
		 * \param devID ID of device to retrieve
		 * \return Returns result of handling data
		 */
		virtual EngineInterface::RESULT handleDeviceInput(const DeviceIdentifier &devID);

		/*!
		 * \brief State of the server. See state_t for description of possible states
		 */
		state_t _state = STOPPED;

		/*!
		 * \brief Device Controllers for individual engines. Mapped from device name to controller
		 */
		std::map<std::string, EngineMPIDeviceControllerInterface*> _deviceControllers;

	private:
		/*!
		 * \brief MPI Communicator to NRP Client
		 */
		MPI_Comm _comm;

		using mutex_t = std::mutex;
		using lock_t = std::unique_lock<mutex_t>;

		/*!
		 * \brief Mutex to prevent multiple threads from accessing _deviceControllers
		 */
		mutex_t _devCtrlLock;
};


template<>
struct MPISinglePropertySerializer<typename EngineMPIControlConst::CommandData> : public MPISinglePropertySerializerGeneral
{
	static mpi_prop_datatype_t<MPIDerivedDatatype> derivedMPIDatatype(EngineMPIControlConst::CommandData &prop);

	static void resize(MPIPropertyData &dat, EngineMPIControlConst::CommandData &prop)
	{
		// Setup variant type. A value >= 0 i means
		const auto newSize = *(dat.CurVarLIt++);
		if(newSize >= 0)
		{
			prop.info = std::string();
			std::get<std::string>(prop.info).resize(newSize);
		}
		else if (newSize == -1)
			prop.info = 0.0f;
		else
			prop.info = 0;
	}

	static void saveSize(MPIPropertyData &dat, EngineMPIControlConst::CommandData &prop)
	{
		// TODO Check if this is portable
		if(std::holds_alternative<float>(prop.info))
			dat.VariableLengths.push_back(-1);
		else if(std::holds_alternative<int64_t>(prop.info))
			dat.VariableLengths.push_back(-2);
		else
			dat.VariableLengths.push_back(std::get<std::string>(prop.info).size()+1);
	}

	static constexpr int getVarSizes()
	{	return 1;	}
};


#endif // ENGINE_MPI_SERVER_H
