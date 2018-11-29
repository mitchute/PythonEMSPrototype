cmake_minimum_required(VERSION 3.9)

project(EnergyPlusPyEMS)

set(CMAKE_CXX_STANDARD 11)

find_package(PythonLibs 3 REQUIRED)

add_executable(EnergyPlusPyEMS
        src/main.cpp
        include/utility.h
        src/utility.cpp
        include/ems_manager.h
        src/ems_manager.cpp)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(include)
target_link_libraries(EnergyPlusPyEMS python3.6)

if(UNIX AND NOT APPLE)
    # I'd like to use the library value string returned by FindPython, but it doesn't have the entire list
    # this is what was returned to me on Ubuntu from `python3-config --ldflags`
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions")
elseif(APPLE)
    # this is what was returned to me on my Mac from `python3-config --ldflags` (I adapted 3.7 to 3.6)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/python/Frameworks/Python.framework/Versions/3.6/lib/python3.6/config-3.6m-darwin -lpython3.6m -ldl -framework CoreFoundation")
else()
    # I do _not_ know if this will work, but whatever
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PYTHON_LIBRARIES}")
endif()