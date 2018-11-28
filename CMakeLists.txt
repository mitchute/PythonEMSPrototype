cmake_minimum_required(VERSION 3.9)
project(EnergyPlusPyEMS)
find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
add_executable(EnergyPlusPyEMS main.cpp)
target_link_libraries(EnergyPlusPyEMS python3.6)
message(${PYTHON_LIBRARIES})
if(UNIX)
    # I'd like to use the library value string returned by FindPython, but it doesn't have the entire list
    # this is what was returned to me on Ubuntu from `python3-config --ldflags`
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions")
elseif(MSVC)
    # I do _not_ know if this will work, but whatever
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PYTHON_LIBRARIES}")
endif()