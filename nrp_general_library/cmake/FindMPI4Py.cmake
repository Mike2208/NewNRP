# Try to find mpi4py
# Will define:
# MPI4Py_FOUND
# MPI4Py_INCLUDE_DIR

# Python dependency
#find_package(Python3 ${MPI4Py_FIND_REQUIRED} COMPONENTS Interpreter)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(NOT MPI4Py_INCLUDE_DIR)
	execute_process(COMMAND
		"${Python3_EXECUTABLE}" "-c" "import mpi4py; print(mpi4py.get_include())"
		OUTPUT_VARIABLE MPI4Py_INCLUDE_DIR
		RESULT_VARIABLE MPI4Py_COMMAND_RESULT
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(MPI4Py_COMMAND_RESULT)
		message("jfa: mpi4py not found")
		set(MPI4Py_FOUND FALSE)
	else(MPI4Py_COMMAND_RESULT)
		if (MPI4Py_INCLUDE_DIR MATCHES "Traceback")
			message("jfa: mpi4py matches traceback")
			## Did not successfully include MPI4Py
			set(MPI4Py_FOUND FALSE)
		else (MPI4Py_INCLUDE_DIR MATCHES "Traceback")
			## successful
			set(MPI4Py_FOUND TRUE)
			set(MPI4Py_INCLUDE_DIR ${MPI4Py_INCLUDE_DIR} CACHE STRING "mpi4py include path")
		endif (MPI4Py_INCLUDE_DIR MATCHES "Traceback")
	endif(MPI4Py_COMMAND_RESULT)
else(NOT MPI4Py_INCLUDE_DIR)
	set(MPI4Py_FOUND TRUE)
endif(NOT MPI4Py_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPI4Py DEFAULT_MSG MPI4Py_INCLUDE_DIR)

if(MPI4Py_FOUND)
	add_library(MPI4Py::MPI4Py INTERFACE IMPORTED)
	target_include_directories(MPI4Py::MPI4Py INTERFACE
		${MPI4Py_INCLUDE_DIR})
endif(MPI4Py_FOUND)
