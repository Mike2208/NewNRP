#ifndef TRANSCeivER_FUNCTION_MANAGER_H
#define TRANSCeivER_FUNCTION_MANAGER_H

#include "nrp_general_library/config/transceiver_function_config.h"
#include "nrp_general_library/transceiver_function/transceiver_function_interpreter.h"

#include "nrp_general_library/utils/ptr_templates.h"

#include <set>
#include <list>

/*!
 * \brief Manages all available/active transfer functions
 */
class TransceiverFunctionManager
{
		/*!
		 * \brief Contains settings for a single TF
		 */
		struct TransceiverFunctionSettings
		        : public TransceiverFunctionConfigSharedPtr
		{
			TransceiverFunctionSettings(const TransceiverFunctionConfigSharedPtr &config);

			/*!
			 * \brief Less comparison. Only used for set container sorting. Compares names of both transfer functions, ensuring uniqueness of names
			 * \param rhs
			 * \return Returns true if name of this object is less than the name of rhs
			 */
			bool operator<(const TransceiverFunctionSettings &rhs) const;
		};

	public:
		using tf_settings_t = std::set<TransceiverFunctionSettings>;
		using tf_results_t = std::list<TransceiverFunctionInterpreter::TFExecutionResult>;

		TransceiverFunctionManager() = default;
		TransceiverFunctionManager(boost::python::dict tfGlobals);

		/*!
		 * \brief Return list of devices that the TFs request
		 * \return Returns container with all requested device IDs
		 */
		EngineInterface::device_identifiers_t updateRequestedDeviceIDs() const;

		/*!
		 * \brief Load TF from given configuration
		 * \param tfConfig TF Configuration
		 * \exception Throws an exception if a TF with the same name is already loaded. Use updateTF to change loaded TFs
		 */
		void loadTF(const TransceiverFunctionConfigSharedPtr &tfConfig);

		/*!
		 * \brief Updates an existing TF or creates a new one
		 * \param Name of old TF
		 * \param tfConfig TF Configuration
		 */
		void updateTF(const TransceiverFunctionConfigSharedPtr &tfConfig);

		/*!
		 * \brief Executes all active TFs and records the results
		 * \param interpreter Python TF interpreter
		 * \return Returns the results of all Transfer Functions
		 */
		tf_results_t executeActiveTFs();

		/*!
		 * \brief Execute all TFs linked to an engine
		 * \param engineName Name of engine
		 * \return Returns results of linked TFs
		 */
		tf_results_t executeActiveLinkedTFs(const std::string &engineName);

		/*!
		 * \brief Is TF active
		 * \param tfName Name of TF
		 * \return Returns true if active, false otherwise.
		 * If TF settings with given name are not stored, returns false
		 */
		bool isActive(const std::string &tfName);

		/*!
		 * \brief Get TF Interpreter
		 */
		TransceiverFunctionInterpreter &getInterpreter();

	private:
		/*!
		 * \brief Set containing TF configurations
		 */
		tf_settings_t _tfSettings;

		/*!
		 * \brief Python Interpreter for TFs
		 */
		TransceiverFunctionInterpreter _tfInterpreter;
};

using TransceiverFunctionManagerSharedPtr = std::shared_ptr<TransceiverFunctionManager>;
using TransceiverFunctionManagerConstSharedPtr = std::shared_ptr<const TransceiverFunctionManager>;


#endif // TRANSCeivER_FUNCTION_MANAGER_H
