set(PYTHON ${Python3_EXECUTABLE})
set(MPI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/mpi4py")
set(MPI_API_SOURCE_FILES "${MPI_SOURCE_DIR}/src/mpi4py.MPI.c")
set(MPI_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/mpi4py")
set(PY_BUILD_CMD_ARGS "setup.py" build --build-base "${MPI_BUILD_DIR}")

add_custom_target(MPI4PyModule ALL
	COMMAND ${PYTHON} ${PY_BUILD_CMD_ARGS}
	BYPRODUCTS ${MPI_BUILD_DIR} ${MPI_API_SOURCE_FILES}
	WORKING_DIRECTORY ${MPI_SOURCE_DIR}
	VERBATIM)

add_library(MPI4PyAPI INTERFACE)
add_dependencies(MPI4PyAPI MPI4PyModule)

target_include_directories(MPI4PyAPI
	INTERFACE
	    $<INSTALL_INTERFACE:${PYTHON_INSTALL_DIR_REL}/mpi4py/include>
		$<BUILD_INTERFACE:${MPI_SOURCE_DIR}/src/mpi4py/include>
		${PYTHON_INCLUDE_DIR}
)

target_link_libraries(MPI4PyAPI
	INTERFACE
	    MPI::MPI_CXX
		${PYTHON_LIBRARIES}
)

add_library(MPI4Py::MPI4Py ALIAS MPI4PyAPI)

install(CODE 
	"execute_process(COMMAND ${PYTHON} ${PY_BUILD_CMD_ARGS} install --prefix ${CMAKE_INSTALL_PREFIX}
	                 WORKING_DIRECTORY ${MPI_SOURCE_DIR})")
