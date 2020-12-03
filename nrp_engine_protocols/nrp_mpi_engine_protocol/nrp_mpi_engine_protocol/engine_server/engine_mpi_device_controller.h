#ifndef ENGINE_MPI_DEVICE_CONTROLLER_H
#define ENGINE_MPI_DEVICE_CONTROLLER_H

#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/engine_interfaces/engine_interface.h"
#include "nrp_general_library/engine_interfaces/engine_mpi_interface/device_interfaces/mpi_device_conversion_mechanism.h"
#include "nrp_general_library/utils/nrp_exceptions.h"
#include "nrp_general_library/utils/serializers/mpi_property_serializer.h"

/*!
 * \brief Controller for single device. Must be registered with corresponding EngineMPIServer
 */
class EngineMPIDeviceControllerInterface
        : public DeviceIdentifier,
          public PtrTemplates<EngineMPIDeviceControllerInterface>
{
	public:
		/*!
		 * \brief Constructor
		 * \param id ID of device that this controller handles
		 */
		EngineMPIDeviceControllerInterface(const DeviceIdentifier &id);
		virtual ~EngineMPIDeviceControllerInterface() = default;

		/*!
		 * \brief Get device data if requested from CLE
		 * \param deviceID ID of device
		 * \return Returns device output as MPIPropertyData
		 */
		virtual MPIPropertyData getDeviceOutput() = 0;

		/*!
		 * \brief Handle device data from CLE. Will receive data over MPI as well
		 * \param deviceInput Device data
		 * \return Returns true on success, false otherwise
		 */
		virtual void handleMPIDeviceInput(MPI_Comm comm, int tag) = 0;
};

/*!
 * \brief Engine Controller with corresponding device information. All MPI controllers must be based on this class
 * \tparam DEVICE DEVICE type that this controller manages
 */
template<DEVICE_C DEVICE>
class EngineMPIDeviceController
        : public EngineMPIDeviceControllerInterface
{
	public:
		EngineMPIDeviceController(const DeviceIdentifier &id)
		    : EngineMPIDeviceControllerInterface(id)
		{}

		virtual ~EngineMPIDeviceController() override = default;

		MPIPropertyData getDeviceOutput() override = 0;

		/*!
		 * \brief Processes device data received from the CLE
		 * \param data Device data
		 * \return Returns SUCCESS or ERROR
		 */
		virtual void handleDeviceInput(DEVICE &data) = 0;

		void handleMPIDeviceInput(MPI_Comm comm, int tag) override final
		{
			DEVICE dev = EngineMPIDeviceController::recvMPIDevice(comm, tag);
			return this->handleDeviceInput(dev);
		}


	private:
		/*!
		 * \brief Receive DEVICE data over MPI
		 * \param comm MPI Communication channel
		 * \param tag MPI tag
		 * \return Returns DEVICE
		 * \exception Throws runtime error on failure
		 */
		static inline DEVICE recvMPIDevice(MPI_Comm comm, int tag)
		{
			DEVICE retVal;

			try
			{	MPICommunication::recvDevice<DEVICE, false>(comm, tag, retVal);	}
			catch(std::exception &e)
			{
				throw NRPException::logCreate(e, "Failed to retrieve device data");
			}

			return retVal;
		}
};

#endif //ENGINE_MPI_DEVICE_CONTROLLER_H
