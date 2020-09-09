# NewNRP

## Dependency Installation

    # Pistache REST Server
    sudo add-apt-repository ppa:pistache+team/unstable

    # Gazebo
    sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
    wget https://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -

    sudo apt update
    sudo apt install cmake libpistache-dev g++-10 libboost-python-dev libboost-filesystem-dev libcurl4-openssl-dev nlohmann-json3-dev libzip-dev libgazebo11-dev gazebo11 gazebo11-plugin-base cython3
    
 - CAN ONLY BE INSTALLED ON UBUNTU 20+ AT THE MOMENT
 - gazebo: For GazeboEngine
 - pistache: REST Server
 - restclient-cpp: REST Client
 - nlohmann_json: JSON Format handling
 - libzip: ZIP File Handling (not used yet)
 - nest: For NestEngine
 - Python: For everything Python
 - Boost-Python: Easier Python handling
 - GTest: Testing Suite
 - spdlog: Logging functions
 - Cython3: Required by Nest

## Installation

 1. `git clone https://github.com/Mike2208/NewNRP.git`
 2. `cd NewNRP`
 3. `mkdir build`
 4. `cd build`
 5. `export C=/usr/bin/gcc-10; export CXX=/usr/bin/g++-10`
 5. `cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/nrp`
 6. `make -j8`
     Note that the installation process might take some time, as it downloads and compiles Nest as well
 7. `sudo mkdir -p /usr/local/nrp`
 8. `sudo chown <USER> /usr/local/nrp`
 9. `make install`

## Running the simulation

 1. Set environment:
 
		 export PYTHONPATH=/usr/local/nrp/lib/python3.8/site-packages:$PYTHONPATH
		 export LD_LIBRARY_PATH=/usr/local/nrp/lib:$LD_LIBRARY_PATH
		 export PATH=$PATH:/usr/local/nrp/bin
      	
 3. Start simulation:
	`NRPSimulation -c <SIMULATION_CONFIG_FILE>`

## Terminology and Classes

 - NRPSimulation: Main Program. Runs the CLE
 - Engine: Child process of NRPSimulation. Will run a single module and communicate with main CLE process. Each Engine is defined by a unique name as well as a type that designates what kind of engine is being run (Gazebo/Nest/Python/...)
	 - EngineServer: Server running in a separate process to facilitate communication between Engine and CLE
	 - EngineDeviceController: Server side device controller. Handles the sending and receiving of devices. Each device requires a controller
	 - NRPClient: CLE-side client that communicates with a single EngineServer
	 - DeviceConversionMechanism: Mechanism to convert a device to/from a communication data type. Currently, it converts devices to/from a JSON structure which can then be exchanged between the EngineServer and NRPClient
 - Device: Data Type used for communication between Engine and CLE. Can be anything, from single integer value, over string, to a mixture of differently-typed arrays
	 - DeviceIdentifier: Unique Identifier for device data. 
	Contains 3 strings:
		 - Name: Name of device. Must be unique to the engine
		 - Engine Name: Name of engine that has sent/will receive the device
		 - Device Type: String identifying the current device type. Will be used to properly de-/serialize the device data
	 - Input and Output: Devices are defined from the viewpoint of the engine. Devices that are sent from the engine to the CLE are considered OutputDevices, devices that are sent to the engine are considered InputDevices (This are merely terms. Programmatically, there is no difference between them).
 - TransceiverFunctionManager and TransceiverFunctionInterpreter: Classes to manage TransceiverFunctions. The TFManager will read TF configuration files and organize the functions appropriately. The TFInterpreter provides Python environments for the TFs to run
 - TransceiverFunctions: Same as TransferFunction. A python script executed inside the CLE to exchange data between engines
	 - TransceiverFunction Decorator: Registers a function with the TransceiverFunctionInterpreter. Users must designate the TF with this decorator
	 - SingleTransceiverDevice Decorator: Registers a device as required by this TF. Similar to how the current CLE TFs work
	 - Return value: A user-defined TF must return a list of devices. These devices will be sent to the appropriate engines
 - SimulationManager: Main Simulation class. Starts the simulation loop, integrates engines, manages TransceiverFunctions
 - SimulationLoop: Main CLE loop. Will call NRPClient of an engine to transfer device data as well as TransceiverFunctions
 - PropertyTemplate: A C++ template class to create devices with structured data. Used to make de-/serialization easier. Also used for config files
 - PluginSystem: Load additional engines on startup by supplying their .so library
 - ProcessLaunchers: Different options can be supplied to launch processes. This is important when switching from a local machine to an HPC cluster with managed ressources, or when integrating MPI

### Current Engine Interface: EngineJSON

 - EngineJSONServer: Server-side engine. 
   Supports the following communication functions:
	 - initialize: After startup, the NRPClient will send an initialize command. Can be used to perform initialization routines
	 - runLoopStep: Run the engine for a given amount of time
	 - shutdown: Stop the engine at end of simulation
	 - getDeviceData: Receives an array of DeviceIdentifiers in JSON format from the NRPClient. Will retrieve the requested devices via the EngineDeviceController and send them back
	 - setDeviceData: Receives an array of Devicedata in JSON format from NRPClient. Will set data in the engine and perform any functions stored inside the corresponding EngineDeviceController
 - EngineJSONNRPClient: CLE-side communicator that sends/receives data to/from the corresponding EngineJSONServer
 - JSONDeviceConversionMechanism:
	 - A type of DeviceConversionMechanism. Will de-/serialize devices from/to JSON format

### Current Engines:

##### Gazebo

 - In subdirectory nrp_gazebo_device_interface
 - Uses an EngineJSONServer
 - Has three device types:
	 - JSONPhysicsCamera
	 - JSONPhysicsJoint
	 - JSONPhysicsLink
 - Uses an unmodified gazebo as base, all modifications are done via plugin
 - Starts gazebo and loads plugins to communicate with the CLE

##### Nest
 - In subdirectory nrp_nest_device_interface
 - Uses an EngineJSONServer
 - Takes a python file as input to define the initialize function (Start nest, define devices, ...)
 - Has single device types:
	 - NestJSONDeviceInterface: Should be able to interface with all Nest devices, as they all use the same format
 - Uses an unmodified Nest as base
 - Start NRPNestExecutable as a child process

##### Python
 - In subdirectory nrp_python_device_interface
 - Uses an EngineJSONServer
 - Takes a python file as input to define the initialize, runStep, and shutdown functions
 - Has single device types:
	 - PythonObjectDeviceInterface: Should be able to interface with many Python datatypes. Uses Python-side JSON converter
 - Starts NRPPythonExecutable as a child process

##### Additional Engines
Additional engines can be defined in a similar manner. 

## Basic Information

 - The project is divided into multiple libraries, separated by folders:
	 - nrp_general_library: Main Library. Contains classes and methods to interface with Python, Engines, and Transceiver-Functions
	 - nrp_nest_device_interface: Nest Engine
	 - nrp_gazebo_device_interface: Gazebo Engine
	 - nrp_python_device_interface: Python Engine
	 - nrp_simulation: Contains the SimulationLoop and -Manager. Creates the NRPSimulation executable
 - Each of these folders also contains a 'tests' folder with basic integration testing capabilities. To run the tests, look for generated executables inside the build folder. Before running the tests, setup the environment as described above in **Running the simulation**
 - All libraries generate a python module. This can be used to interface with the devices from the TFs. After installation, they will be located inside `/usr/local/nrp/lib/python3.8/site-packages`
 - Both nrp_nest_device_interface and nrp_python_device_interface create executables that can be started as child processes of the main NRP

## Examples

 - Examples are located in the examples subfolder:
	 - To run them, first set the environment as described in **Running the simulation**. Then:

			cd examples/<EXAMPLE_NAME>
			NRPSimulation -c <SIMULATION_CONFIG> -p "NRPPythonDeviceInterface.so"
			
	 - If gazebo is running in the experiment, you can use `gzclient` to view what's happening

