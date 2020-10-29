#include "nrp_nest_server_engine/engine_server/nest_server.h"

#include "nrp_general_library/utils/python_error_handler.h"
#include "nrp_general_library/utils/python_interpreter_state.h"

#include "nrp_nest_server_engine/config/nrp_nest_cmake_constants.h"
#include "nrp_nest_server_engine/engine_server/nest_engine_device_controller.h"
#include "nrp_nest_server_engine/python/create_device_class.h"

#include <fstream>

namespace python = boost::python;


NestServer::NestServer(const std::string &serverAddress, python::dict globals, python::object locals)
    : EngineJSONServer(serverAddress),
      _pyGlobals(globals),
      _pyLocals(locals)
{}

NestServer::NestServer(const std::string &serverAddress, const std::string &engineName, const std::string &registrationAddress, python::dict globals, boost::python::object locals)
    : EngineJSONServer(serverAddress, engineName, registrationAddress),
      _pyGlobals(globals),
      _pyLocals(locals)
{}

NestServer::~NestServer()
{
	try
	{
		// If Nest has been initialized, run cleanup
		if(this->_nestPreparedFlag && this->_pyNest.has_key("Cleanup"))
		{
			this->_nestPreparedFlag = false;
			this->_pyNest["Cleanup"]();
		}

		// Shutdown any running threads
		this->_shutdownFlag = true;
	}
	catch(python::error_already_set &)
	{
		// If an error occured, print the error
		PyErr_Print();
		PyErr_Clear();
	}
}

bool NestServer::initRunFlag() const
{
	return this->_initRunFlag;
}

bool NestServer::shutdownFlag() const
{
	return this->_shutdownFlag;
}

float NestServer::runLoopStep(float timeStep)
{
	PythonGILLock lock(this->_pyGILState, true);

	try
	{
		//
		const auto resolution = python::extract<double>(this->_pyNest["GetKernelStatus"]("resolution"));
		const double runTime = std::round(NestServer::convertSecToMill(timeStep)/resolution)*resolution;
		this->_pyNest["Run"](runTime);
		return NestServer::convertMillToSec(python::extract<float>(this->_pyNest["GetKernelStatus"]("time")));
	}
	catch(python::error_already_set &e)
	{
		// If an error occured, print the error
		PyErr_Print();
		PyErr_Clear();

		throw e;
	}
}

nlohmann::json NestServer::initialize(const nlohmann::json &data, EngineJSONServer::lock_t&)
{
	PythonGILLock lock(this->_pyGILState, true);
	try
	{
		python::object nestModule = python::import("nest");
		this->_pyNest = python::dict(nestModule.attr("__dict__"));

		python::object nrpNestModule = python::import(NRP_NEST_PYTHON_MODULE_STR);
		this->_pyNRPNest = python::dict(nrpNestModule.attr("__dict__"));

		this->_pyGlobals["nest"] = nestModule;
		this->_pyGlobals[NRP_NEST_PYTHON_MODULE_STR] = nrpNestModule;
	}
	catch(python::error_already_set &)
	{
		// If an error occured, return the message to the NRP server without setting the initRunFlag
		if (PyErr_Occurred())
		{
			const auto msg = handle_pyerror();
			PyErr_Clear();

			std::cerr << msg;
			return this->formatInitErrorMessage(msg);
		}
		PyErr_Clear();
	}

	// Read received configuration
	const NestServerConfig config(data.at(NestServerConfig::ConfigType.m_data));

	// Empty device mapping
	this->_devMap.clear();

	// Read init file if present
	const auto &initFileName = config.nestInitFileName();
	if(!initFileName.empty())
	{
		std::fstream initFile(initFileName, std::ios_base::in);
		if(!initFile.is_open())
		{
			const auto errMsg = "Could not find init file " + initFileName;
			std::cerr << errMsg;
			throw std::invalid_argument(errMsg);
		}
		initFile.close();

		// Execute Init File
		try
		{
			python::exec_file(python::str(initFileName), this->_pyGlobals, this->_pyLocals);
		}
		catch(python::error_already_set &)
		{
			// If an error occured, return the message to the NRP server without setting the initRunFlag
			if (PyErr_Occurred())
			{
				const auto msg = handle_pyerror();
				PyErr_Clear();

				std::cerr << msg;
				return this->formatInitErrorMessage(msg);
			}
		}
	}

	nlohmann::json jsonDevMap;
	try
	{
		// Read device map
		python::dict jsonModule = static_cast<python::dict>(python::import("json").attr("__dict__"));
		python::object jsonSerialize = jsonModule["dumps"];

		const std::string jsonStr = python::extract<std::string>(jsonSerialize(this->_devMap));
		jsonDevMap = nlohmann::json::parse(jsonStr);

		// Register devices
		this->_devMap = python::dict(this->_pyNRPNest["GetDevMap"]());
		python::list devMapKeys = this->_devMap.keys();
		const long numDevices = python::len(devMapKeys);
		//const long numDevices = python::len(this->_pyNRPNest["GetDevMap"]());
		for(long i=0; i < numDevices; ++i)
		{
			const python::object &devKey = devMapKeys[i];
			const std::string devName = python::extract<std::string>(python::str(devKey));
			python::object devNodes = this->_devMap[devKey];

			auto devController = std::shared_ptr<NestEngineJSONDeviceController<NestServerDevice> >(new
			            NestEngineJSONDeviceController<NestServerDevice>(DeviceIdentifier(devName, NestServerDevice::TypeName.data(), config.engineName()),
												 devNodes, this->_pyNest));

			this->_deviceControllerPtrs.push_back(devController);
			this->registerDeviceNoLock(devName, devController.get());
		}

		// Prepare Nest for execution
		this->_pyNest["Prepare"]();
		this->_nestPreparedFlag = true;
	}
	catch(python::error_already_set &)
	{
		// If an error occured, print the error
		if (PyErr_Occurred())
		{
			const auto msg = handle_pyerror();
			PyErr_Clear();

			std::cerr << msg;
			return this->formatInitErrorMessage(msg);
		}
		PyErr_Clear();
	}

	// Init has run once
	this->_initRunFlag = true;

	// Return success and parsed devmap
	return nlohmann::json({{NestServerConfig::InitFileExecStatus, true}, {NestServerConfig::InitFileParseDevMap, jsonDevMap}});
}

nlohmann::json NestServer::shutdown(const nlohmann::json &)
{
	PythonGILLock lock(this->_pyGILState, true);

	this->_shutdownFlag = true;

	if(this->_nestPreparedFlag)
	{
		this->_nestPreparedFlag = false;
		this->_pyNest["Cleanup"]();
	}

	// Remove device controllers
	this->clearRegisteredDevices();
	this->_deviceControllerPtrs.clear();

	return nlohmann::json();
}

constexpr float NestServer::convertSecToMill(const float sec)
{
	return std::chrono::duration<decltype(sec), std::ratio<1,1000> >(std::chrono::duration<decltype(sec)>(sec)).count();
}

constexpr float NestServer::convertMillToSec(const float millsec)
{
	return std::chrono::duration<decltype(millsec), std::ratio<1000,1> >(std::chrono::duration<decltype(millsec)>(millsec)).count();
}

nlohmann::json NestServer::formatInitErrorMessage(const std::string &errMsg)
{
	return nlohmann::json({{NestServerConfig::InitFileExecStatus, 0}, {NestServerConfig::InitFileErrorMsg, errMsg}});
}

nlohmann::json NestServer::getDeviceData(const nlohmann::json &reqData)
{
	PythonGILLock lock(this->_pyGILState, true);
	return this->EngineJSONServer::getDeviceData(reqData);
}

nlohmann::json NestServer::setDeviceData(const nlohmann::json &reqData)
{
	PythonGILLock lock(this->_pyGILState, true);
	return this->EngineJSONServer::setDeviceData(reqData);
}
