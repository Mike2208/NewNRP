find_package(Python3 REQUIRED)
set(PYTHON ${Python3_EXECUTABLE})

include(FetchContent)
FetchContent_Declare(
	mpi4py
	GIT_REPOSITORY https://bitbucket.org/mpi4py/mpi4py.git
	GIT_TAG        3.0.3
	)

FetchContent_GetProperties(mpi4py)
if(NOT ${mpi4py_POPULATED})
	message("MPI4Py not found on system. Please wait. Downloading mpi4py...")
	FetchContent_Populate(mpi4py)

	set(MPI_API_SOURCE_FILES "${mpi4py_SOURCE_DIR}/src/mpi4py.MPI.c")
	set(PY_BUILD_CMD_ARGS "setup.py" build --build-base "${mpi4py_BINARY_DIR}")

	add_custom_target(MPI4PyModule ALL
		COMMAND ${PYTHON} ${PY_BUILD_CMD_ARGS}
		BYPRODUCTS ${mpi4py_BINARY_DIR} ${MPI_API_SOURCE_FILES}
		WORKING_DIRECTORY ${mpi4py_SOURCE_DIR}
		VERBATIM)

	add_library(MPI4PyAPI INTERFACE)
	add_dependencies(MPI4PyAPI MPI4PyModule)

	target_include_directories(MPI4PyAPI INTERFACE
	    $<INSTALL_INTERFACE:${PYTHON_INSTALL_DIR_REL}/mpi4py/include>
		$<BUILD_INTERFACE:${mpi4py_SOURCE_DIR}/src/mpi4py/include>
		${PYTHON_INCLUDE_DIR})

	target_link_libraries(MPI4PyAPI	INTERFACE
		MPI::MPI_CXX
		${PYTHON_LIBRARIES})

	add_library(MPI4Py::MPI4Py ALIAS MPI4PyAPI)
endif()

install(CODE 
	"execute_process(COMMAND ${PYTHON} ${PY_BUILD_CMD_ARGS} install --prefix ${CMAKE_INSTALL_PREFIX}
     WORKING_DIRECTORY ${mpi4py_SOURCE_DIR})")
