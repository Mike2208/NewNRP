/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef ENGINE_INTERFACE_H
#define ENGINE_INTERFACE_H

#include "nrp_general_library/config/engine_config.h"
#include "nrp_general_library/device_interface/device.h"
#include "nrp_general_library/process_launchers/process_launcher.h"
#include "nrp_general_library/utils/fixed_string.h"
#include "nrp_general_library/utils/ptr_templates.h"
#include "nrp_general_library/utils/time_utils.h"

#include <concepts>
#include <set>
#include <vector>

class EngineInterface;
class EngineLauncherInterface;

template<class T>
concept ENGINE_C = requires (EngineConfigConst::config_storage_t &config, ProcessLauncherInterface::unique_ptr &&launcher) {
    std::derived_from<T, EngineInterface>;
    //std::constructible_from<T, EngineConfigConst::config_storage_t&, ProcessLauncherInterface::unique_ptr&&>;
    //{ T(config, std::move(launcher)) };
};

/*!
 * \brief Interface to engines
 */
class EngineInterface
        : public PtrTemplates<EngineInterface>
{
		/*! \brief DeviceInterfaceConstSharedPtr comparison. Used for set sorting */
		struct CompareDevInt : public std::less<>
		{
			public: bool operator()(const DeviceInterfaceConstSharedPtr &lhs, const DeviceInterfaceConstSharedPtr &rhs) const
			{	return lhs->name() < rhs->name();	}

			public: bool operator()(const DeviceInterfaceConstSharedPtr &lhs, const std::string &name) const
			{	return lhs->name() < name;	}

			public: bool operator()(const std::string &name, const DeviceInterfaceConstSharedPtr &rhs) const
			{	return name < rhs->name();	}
		};

	public:
		using device_identifiers_t = std::set<DeviceIdentifier>;
		using device_outputs_t = std::vector<DeviceInterfaceConstSharedPtr>;
		using device_outputs_set_t = std::set<DeviceInterfaceConstSharedPtr, CompareDevInt>;
		using device_inputs_t = std::vector<DeviceInterface*>;

		explicit EngineInterface(ProcessLauncherInterface::unique_ptr &&launcher);
		virtual ~EngineInterface();

		/*!
		 * \brief Get Engine Name
		 * \return Returns engine name
		 */
		const std::string &engineName() const;

		/*!
		 * \brief Get Engine Name
		 * \return Returns engine name
		 */
		std::string &engineName();

		/*!
		 * \brief Get engine config general data
		 */
		virtual std::shared_ptr<EngineConfigGeneral> engineConfigGeneral() const = 0;

		/*!
		 * \brief Launch the engine
		 * \return Returns engine process ID on success, throws on failure
		 */
		virtual pid_t launchEngine();

		/*!
		 * \brief Initialize engine
		 * \return Returns SUCCESS if no error was encountered
		 * \throw Throws on error
		 */
		virtual void initialize() = 0;

		/*!
		 * \brief Shutdown engine
		 * \return Return SUCCESS if no error was encountered
		 * \throw Throws on error
		 */
		virtual void shutdown() = 0;

		/*!
		 * \brief Get engine timestep (in seconds)
		 * \throw Throws on error
		 */
		virtual SimulationTime getEngineTimestep() const = 0;

		/*!
		 * \brief Get current engine time (in seconds)
		 * \return Returns engine time
		 * \throw Throws on error
		 */
		virtual SimulationTime getEngineTime() const = 0;

		/*!
		 * \brief Starts a single loop step in a separate thread.
		 * EngineInterface::waitForStepCompletion() can be used to join threads again
		 * \param timeStep Time (in seconds) of a single step
		 * \throw Throws on error
		 */
		virtual void runLoopStep(SimulationTime timeStep) = 0;

		/*!
		 * \brief Wait until step has been completed, at most timeOut seconds
		 * \param timeOut Wait for at most timeOut seconds
		 * \return Returns SUCCESS if step has completed before timeOut, ERROR otherwise
		 * \throw Throws on error
		 */
		virtual void waitForStepCompletion(float timeOut) = 0;

		/*!
		 * \brief Gets requested output devices from physics simulator.
		 * Uses requestOutputDeviceCallback override for actual communication and stores received data in _deviceCache
		 * \param deviceNames All requested names. NOTE: can also include IDs of other engines. A check must be added that only the corresponding IDs are retrieved
		 * \return Returns all requested output devices
		 */
		const device_outputs_t &requestOutputDevices(const device_identifiers_t &deviceIdentifiers);

		/*!
		 * \brief get cached Engine OutputDevices
		 */
		constexpr const device_outputs_t &getOutputDevices() const
		{	return this->_deviceCache;	}

		/*!
		 * \brief Handles received input devices
		 * \param inputDevices All input devices that the phyiscs simulation should process
		 * \return Returns SUCCESS if all input devices could be handles, ERROR otherwise
		 * \throw Throws on error
		 */
		virtual void handleInputDevices(const device_inputs_t &inputDevices) = 0;

	protected:

		/*!
		 * \brief Gets requested output devices from physics simulator
		 * \param deviceNames All requested names. NOTE: can also include IDs of other engines. A check must be added that only the corresponding IDs are retrieved
		 * \return Returns all requested output devices
		 * \throw Throws on error
		 */
		virtual device_outputs_set_t requestOutputDeviceCallback(const device_identifiers_t &deviceIdentifiers) = 0;

		/*!
		 * \brief Process Launcher. Will be used to stop process at end
		 */
		ProcessLauncherInterface::unique_ptr _process;

		/*!
		 * \brief Engine device cache. Stores retrieved devices
		 */
		device_outputs_t _deviceCache;

		/*!
		 * \brief Insert sorted devices into _deviceCache
		 * \param devs Devices to insert
		 */
		void insertSorted(device_outputs_set_t &&devs);
};

using EngineInterfaceSharedPtr = EngineInterface::shared_ptr;
using EngineInterfaceConstSharedPtr = EngineInterface::const_shared_ptr;

class EngineLauncherInterface
        : public PtrTemplates<EngineLauncherInterface>
{
	public:
		using engine_type_t = decltype(DeviceIdentifier::Type);

		EngineLauncherInterface(const engine_type_t &engineType);
		virtual ~EngineLauncherInterface() = default;

		const engine_type_t &engineType() const;
		virtual EngineInterfaceSharedPtr launchEngine(EngineConfigConst::config_storage_t &configHolder, ProcessLauncherInterface::unique_ptr &&launcher) = 0;

		/*!
		 *	\brief Compare EngineLaunchers according to _engineType
		 */
		auto operator<=>(const EngineLauncherInterface&) const = default;
		bool operator==(const EngineLauncherInterface&) const = default;
	private:
		/*!
		 * \brief Engine Type
		 */
		const engine_type_t _engineType;
};

using EngineLauncherInterfaceSharedPtr = EngineLauncherInterface::shared_ptr;
using EngineLauncherInterfaceConstSharedPtr = EngineLauncherInterface::const_shared_ptr;

/*!
 *	\brief Base class for all Engines
 *	\tparam ENGINE Final derived engine class
 *	\tparam ENGINE_CONFIG Config class corresponding to ENGINE
 */
template<class ENGINE, ENGINE_CONFIG_C ENGINE_CONFIG>
class Engine
        : private std::shared_ptr<ENGINE_CONFIG>,
          public EngineInterface
{
	public:
		using engine_t = ENGINE;
		using engine_config_t = ENGINE_CONFIG;

		/*!
		 * \brief Class for launching engine
		 * \tparam ENGINE_TYPE Default engine type
		 */
		template<FixedString ENGINE_TYPE>
		class EngineLauncher
		        : public EngineLauncherInterface
		{
			public:
				EngineLauncher()
				    : EngineLauncherInterface(ENGINE_TYPE.m_data)
				{}

				EngineLauncher(const engine_type_t &engineType)
				    : EngineLauncherInterface(engineType)
				{}

				~EngineLauncher() override = default;

				/*!
				 * \brief Launches an engine. Configures config and forks a new child process for the engine
				 * \param configHolder Engine Configuration
				 * \param launcher Process Forker
				 * \return Returns pointer to EngineInterface
				 */
				EngineInterfaceSharedPtr launchEngine(EngineConfigConst::config_storage_t &configHolder, ProcessLauncherInterface::unique_ptr &&launcher) override
				{
					EngineInterfaceSharedPtr engine(new ENGINE(configHolder, std::move(launcher)));
					if(engine->launchEngine() == 0)
					{	/*TODO: Handle process forking error*/	}

					return engine;
				}
		};

		/*!
		 * \brief Constructor
		 * \param configHolder Engine Configuration
		 * \param launcher Process Forker
		 */
		Engine(EngineConfigConst::config_storage_t &configHolder, ProcessLauncherInterface::unique_ptr &&launcher)
		    : std::shared_ptr<ENGINE_CONFIG>(new ENGINE_CONFIG(configHolder)),
		      EngineInterface(std::move(launcher))
		{}

		~Engine() override = default;

		SimulationTime getEngineTimestep() const override final
		{
			// We need to cast floating-point seconds to integers with units of SimulationTime type

			return toSimulationTime<float, std::ratio<1>>(this->engineConfigGeneral()->engineTimestep());
		}

		/*!
		 * \brief Get General Engine Configuration
		 */
		std::shared_ptr<EngineConfigGeneral> engineConfigGeneral() const override
		{	return this->engineConfig();	}

		/*!
		 * \brief Get Engine Configuration
		 */
		const std::shared_ptr<ENGINE_CONFIG> &engineConfig() const
		{	return static_cast<const std::shared_ptr<ENGINE_CONFIG> &>(*this);	}
};


/*! \page engines "Engines"
Engines are the core aspect of NRP Simulation. They run the actual simulation software, with the SimulationLoop and TransceiverFunctions merely being a way to synchronize and
exchange data between them. An Engine can run any type of software, from physics engines to brain simulators.

From the NRP's perspective, the core component of the engine is its communication interface, which allows it to communicate with the SimulationLoop. Different engine types can
have different communication protocols. Nevertheless, all protocols are envisioned as a server-client architecture, with the Engine server running as a separate process, and a
client running inside the NRPSimulation. As such, all Engines must at least support the following functionality:

- LaunchEngine: A function to launch the engine process. This will usually in some way use the ProcessLauncherInterface
- Initialize: A function that initializes the engine after launch
- RunLoopStep: A function that will advance the engine for a given timestep
- RequestOutputDevices: A function to retrieve Device data from the Engine
- HandleInputDevices: A function to handle incoming Device data
- Shutdown: A function that gracefully stops the Engine

The \ref index "Main Page" has a list of currently supported Engines.

Creating new engines is a process that requires multiple components to work together. Should you be interested in implementing your own engine, a good starting point is
\ref tutorial_engine_creation "this tutorial". An easiser approach is to use one of our already implemented Grpc communication protocol and adapt it to your simulator.
The tutorial can be found \ref grpc_engine_creation "here".

\subsection Engine Launchers

An EngineLauncher is in charge of properly launching an engine with a given ProcessLauncher and Engine Configuration. Most of the time, the routine is fairly straightforward.
For these instances, we have provided an already configured class that can be used out-of-the-box, within any newly created Engine.
\code{.cpp}
// Define the EngineLauncher.
using NewEngineLauncher = NewEngine::EngineLauncher<NewEngineConfig::DefEngineType>;
\endcode

A new Engine library can use such a `NewEngineLauncher` to make it plugin compatible. Look under \ref plugin_system for additional details.

Note that we assign this EngineLauncher the name specified in NewEngineConfig::DefEngineType. Thus, a user can select this engine in the main simulation configuration file by
setting an Engine's EngineType parameter to the string specified in `NewEngineConfig::DefEngineType`. For details about setting up a simulation configuration file, look
\ref simulation_config "here".

For details about how to create a `NewEngineConfig` class, see section \ref config.
 */

#endif // ENGINE_INTERFACE_H
