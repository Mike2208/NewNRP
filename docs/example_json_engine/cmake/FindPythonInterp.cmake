# Override deprecated PythonInterp for nest simulator
find_package(Python3 REQUIRED)
set(PYTHON_FOUND ${Python3_FOUND})
set(PYTHON_VERSION_MAJOR ${Python3_VERSION_MAJOR})
set(PYTHON_VERSION_MINOR ${Python3_VERSION_MINOR})
set(PYTHONINTERP_FOUND ${Python3_Interpreter_FOUND})
set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
set(PYTHON_VERSION_STRING ${Python3_VERSION})

