#ifndef ENERGYPLUSPYEMS_EMS_MANAGER_H
#define ENERGYPLUSPYEMS_EMS_MANAGER_H

#include <string>
#include <vector>

#ifdef _DEBUG
// We don't want to try to import a debug build of Python here
// so if we are building a Debug build of the C++ code, we need
// to undefine _DEBUG during the #include command for Python.h.
// Otherwise it will fail
#undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
#include <Python.h>
#endif

#include <simulation.h>
#include <utility.h>

enum class CallingPoint {
    // hardwired because this would need to get exposed to EMS program writers if calling_point is an argument
    // DO NOT use negative values here, the PyLong_AsLong function returns -1 as an erroneous flag, and we'd have to add add'l error checking (PyErr_Occurred())
    // this is mirrored in the Python code as py_ems_interface.CallingPointMirror class
    AFTER_SIZING = 0,
    HVAC_TIME_STEP_LOOP = 1
};

struct PluginInstance {
    PluginInstance() = default;
    CallingPoint callingPoint;  // which EMS calling point triggers this EMS function call
    std::string stringIdentifier; // for diagnostic reporting
    PyObject *pPluginMainFunction;  // pointer to the main EMS function
    PyObject *pPluginUpdateSensorFunction;  // pointer to the function to update the sensor value
    std::vector<std::string> sensorIDs;  // a list of sensor IDs - currently strings, but in the future integer-ish
    std::vector<std::string> actuatorIDs;  // a list of actuator IDs - currently strings, but in the future integer-ish
};

class PluginManager {
public:
    std::vector<PluginInstance> pluginInstances;
    PluginManager() {
        printCpp("Inside PluginManager constructor, about to call initialize");
        // from https://docs.python.org/3/c-api/init.html
        // If arg 0, it skips init registration of signal handlers, which might be useful when Python is embedded.
        Py_InitializeEx(0);
        PyRun_SimpleString("import sys");  // allows us to report sys.path later
        printCpp("Inside PluginManager constructor, completed call to initialize");
    }
    ~PluginManager() {
        printCpp("Inside PluginManager destructor, about to call finalize");
        Py_FinalizeEx();
        printCpp("Inside PluginManager destructor, completed call to finalize");
    }
    int addToPythonPath(std::string path);
    int initPluginInstanceFromClass(std::string fileName, std::string functionName);
    int callPluginInstances(CallingPoint callingPoint, SensedVariables &sensors, ActuatedVariables &actuators);
};

#endif //ENERGYPLUSPYEMS_EMS_MANAGER_H
