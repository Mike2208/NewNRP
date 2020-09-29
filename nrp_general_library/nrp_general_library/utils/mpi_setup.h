#ifndef MPI_SETUP_H
#define MPI_SETUP_H

#include <memory>

/*!
 * \brief Singleton to setup MPI and MPI4Py before first use
 */
class MPISetup
{
	public:
		/*!
		 * \brief Get Singleton Instance
		 * \return Returns nullptr if not yet initalized
		 */
		static MPISetup *getInstance();

		/*!
		 * \brief Reset Singleton Instance
		 * \param argc MPI_Init argument
		 * \param argv MPI_Init argument
		 */
		static MPISetup *resetInstance(int argc, char **argv);

		/*!
		 * \brief Initialize MPI if not yet done
		 * \param argc MPI_Init argument
		 * \param argv MPI_Init argument
		 */
		static MPISetup *initializeOnce(int argc, char **argv);

		/*!
		 * \brief Get error string for an MPI Error Code
		 * \param MPIErrorCode Error Code
		 * \return Returns string
		 */
		static std::string getErrorString(int MPIErrorCode);

		/*!
		 * \brief Finalizes MPI
		 */
		~MPISetup();

		// Delete for singleton
		MPISetup(const MPISetup&) = delete;
		MPISetup(MPISetup&&) = delete;

		MPISetup &operator=(const MPISetup&) = delete;
		MPISetup &operator=(MPISetup&&) = delete;

	private:
		/*!
		 * \brief Initializes MPI
		 * \param argc
		 * \param argv
		 */
		MPISetup(int argc, char **argv);

		/*!
		 * \brief Singleton instance
		 */
		static std::unique_ptr<MPISetup> _instance;
};

#endif // MPI_SETUP_H
