cmake_minimum_required(VERSION 3.8)

project(FakeEnergyPlus)

set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    # hopefully these hints will help FindPython library -- erm -- find ... Python.
    set(PYTHON_LIBRARY "C:/Python37/libs/python37.lib")
    set(PYTHON_INCLUDE_DIR "C:/Python37/include")
endif()

find_package(PythonLibs 3 REQUIRED)

add_library(FakeAPI SHARED
        include/api.h
        src/api.cpp
)

add_executable(FakeEnergyPlus
        src/main.cpp
        include/utility.h
        src/utility.cpp
        include/ems_manager.h
        src/ems_manager.cpp
        include/simulation.h
        src/file_system.cpp
        include/file_system.h
)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(include)

if(UNIX AND NOT APPLE)
    target_link_libraries(FakeEnergyPlus python3.6 FakeAPI)
    # I'd like to use the library value string returned by FindPython, but it doesn't have the entire list
    # this is what was returned to me on Ubuntu from `python3-config --ldflags`
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions")
elseif(APPLE AND TRAVIS)
    # Travis uses Python 3.6 on Mac by default
    target_link_libraries(FakeEnergyPlus python3.6 FakeAPI)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/python/Frameworks/Python.framework/Versions/3.6/lib/python3.6/config-3.6m-darwin -lpython3.6m -ldl -framework CoreFoundation")
elseif(APPLE)
    # I use 3.7 on my Mac locally
    # this is what was returned to me on my Mac from `python3-config --ldflags`
    target_link_libraries(FakeEnergyPlus python3.7 FakeAPI)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/python/Frameworks/Python.framework/Versions/3.7/lib/python3.7/config-3.7m-darwin -lpython3.7m -ldl -framework CoreFoundation")
else()
    # on Windows (well, on Travis-Windows) Chocolately will install python 3.7
    # target_link_libraries(EnergyPlusPyEMS python3.7)
    # I do _not_ know if this will work, but whatever
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PYTHON_LIBRARIES}")
endif()

# this brings in just the python package subdirectory of the pyms submodule
add_custom_command(
    TARGET FakeEnergyPlus
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/plugin_interface/energyplus_plugin $<TARGET_FILE_DIR:FakeEnergyPlus>/energyplus_plugin
    COMMENT "Copying PyMS directory to build folder"
)
