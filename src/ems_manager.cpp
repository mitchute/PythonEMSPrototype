#include <iostream>
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

#include <ems_manager.h>
#include <simulation.h>
#include <utility.h>

std::string const mainFunctionName = "main";
std::string const callingPointFunctionName = "get_calling_point";
std::string const getSensorsFunctionName = "get_sensed_data_list";
std::string const getActuatorsFunctionName = "get_actuator_list";
std::string const updateSensorFunctionName = "update_sensed_datum";

int PluginManager::initPluginInstanceFromClass(std::string moduleName, std::string className) {

    PluginInstance thisPlugin;

    // this first section is really all about just ultimately getting a full Python class instance
    // this answer helped with a few things: https://ru.stackoverflow.com/a/785927

    PyObject *pModuleName = PyUnicode_DecodeFSDefault(moduleName.c_str());
    PyObject *pModule = PyImport_Import(pModuleName);
    // PyUnicode_DecodeFSDefault documentation does not explicitly say whether it returns a new or borrowed reference,
    // but other functions in that section say they return a new reference, and that makes sense to me, so I think we
    // should decrement it.
    Py_DECREF(pModuleName);
    if (!pModule) {
        printCpp("Failed to import module \"" + moduleName + "\"");
        return 1;
    }
    PyObject *pModuleDict = PyModule_GetDict(pModule);
    Py_DECREF(pModule);  // PyImport_Import returns a new reference, decrement it
    if (!pModuleDict) {
        printCpp("Failed to read module dictionary from module \"" + moduleName + "\"");
        return 1;
    }
    PyObject *pClass = PyDict_GetItemString(pModuleDict, className.c_str());
    // Py_DECREF(pModuleDict);  // PyModule_GetDict returns a borrowed reference, DO NOT decrement
    if (!pClass) {
        printCpp("Failed to get class type \"" + className + "\" from module \"" + moduleName + "\"");
        return 1;
    }
    if (!PyCallable_Check(pClass)) {
        printCpp("Got class type \"" + className + "\", but it cannot be called/instantiated");
        return 1;
    }
    PyObject *pClassInstance = PyObject_CallObject(pClass, NULL);
    // Py_DECREF(pClass);  // PyDict_GetItemString returns a borrowed reference, DO NOT decrement
    if (!pClassInstance) {
        printCpp("Something went awry calling class constructor for class \"" + className + "\"");
        return 1;
    }
    // PyObject_CallObject returns a new reference, that we need to manage
    // I think we need to keep it around in memory though so the class methods can be called later on,
    // so I don't intend on decrementing it, at least not until the manager destructor
    // In any case, it will be an **extremely** tiny memory use if we hold onto it a bit too long

    // now grab the function pointers to each of the relevant functions
    PyObject *pPluginMainFunction = PyObject_GetAttrString(pClassInstance, mainFunctionName.c_str());
    if (!pPluginMainFunction || !PyCallable_Check(pPluginMainFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + mainFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pPluginGetSensorsFunction = PyObject_GetAttrString(pClassInstance, getSensorsFunctionName.c_str());
    if (!pPluginGetSensorsFunction || !PyCallable_Check(pPluginGetSensorsFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + getSensorsFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pPluginUpdateSensorFunction = PyObject_GetAttrString(pClassInstance, updateSensorFunctionName.c_str());
    if (!pPluginUpdateSensorFunction || !PyCallable_Check(pPluginUpdateSensorFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + updateSensorFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pPluginCallingPointFunction = PyObject_GetAttrString(pClassInstance, callingPointFunctionName.c_str());
    if (!pPluginCallingPointFunction || !PyCallable_Check(pPluginCallingPointFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + callingPointFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pPluginGetActuatorsFunction = PyObject_GetAttrString(pClassInstance, getActuatorsFunctionName.c_str());
    if (!pPluginGetActuatorsFunction || !PyCallable_Check(pPluginGetActuatorsFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + getActuatorsFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }

    // we will call to get the calling point for this EPS class, then we're done with that function
    PyObject *pCallingPointResponse = PyObject_CallFunction(pPluginCallingPointFunction, NULL);
    if (!pCallingPointResponse) {
        printCpp("Could not call get_calling_point function on class \"" + moduleName + "." + className + ", make sure it is defined and error free");
        return 1;
    }
    long callingPointInt = PyLong_AsLong(pCallingPointResponse);
    if (callingPointInt == -1) {
        printCpp("Invalid return from get_calling_point on class \"" + moduleName + "." + className + ", make sure it returns a CallingPoint enum integer");
        return 1;
    }
    CallingPoint thisCallingPoint = (CallingPoint) callingPointInt;
    Py_DECREF(pPluginCallingPointFunction);  // PyObject_GetAttrString returns a new reference, decrement it
    Py_DECREF(pCallingPointResponse);  // PyObject_CallFunction returns a new reference, decrement it

    // we also call to get the list of sensors, then we're done with that function
    PyObject *pGetSensorsResponse = PyObject_CallFunction(pPluginGetSensorsFunction, NULL);
    if (!pGetSensorsResponse) {
        printCpp("Could not call get_sensed_data_list function on EPS derived class, make sure it is defined and error free");
        return 1;
    }
    if (PyList_Check(pGetSensorsResponse)) {
        for (Py_ssize_t i = 0; i < PyList_Size(pGetSensorsResponse); ++i) {
            printCpp("Processing return value from \"" + moduleName + "." + className + "." + getSensorsFunctionName + "; item #" + std::to_string(i));
            PyObject *next = PyList_GetItem(pGetSensorsResponse, i);
            std::string thisString = "";
            try {
                // TODO: Use PyInt_Check, PyFloat_Check or whatever here, PyBytes_Check?
                // TODO: Figure out why this isn't caught.
                // It throws a std::logic_error if you change the return value of a get_sensed_data_list to non-strings, like [2.3, 2.1]
                // But wrapping it in a try block doesn't actually catch it.  I read online it could be due to a difference in compiler from
                // what I am currently using to compile this C++ and what they used to compile the underlying Python library
                thisString = PyUnicode_AsUTF8(next);
            } catch (...) {
                printCpp("Invalid string return value from \"" + moduleName + "." + className + "." + getSensorsFunctionName + "; item #" + std::to_string(i));
                return 1;
            }
            thisPlugin.sensorIDs.push_back(thisString);
            // calling the update_sensor function - it will verify functionality and also initialize the dict
            PyObject *pUpdateFunctionResponse = PyObject_CallFunction(pPluginUpdateSensorFunction, "(sd)", thisString.c_str(), 0.0);
            if (!pUpdateFunctionResponse) {
                printCpp("Could not call to update sensor data function, this is a base class function so either it's improperly overridden or EPS class does not inherit EnergyPlusPlugin base class");
                return 1;
            }
            // Py_DECREF(next);  // PyList_GetItem returns a borrowed reference, DO NOT decrement!
            Py_DECREF(pUpdateFunctionResponse);  // PyObject_CallFunction returns a new reference, decrement it
        }
    } else {
        printCpp("Invalid return from get_sensed_data_list on class \"" + moduleName + "." + className + ", make sure it returns a list of strings");
        return 1;
    }
    Py_DECREF(pPluginGetSensorsFunction);  // PyObject_GetAttrString returns a new reference, decrement it
    Py_DECREF(pGetSensorsResponse);  // PyObject_CallFunction returns a new reference, decrement it

    // finally we also call to get the list of actuators, and we're done with that function also
    PyObject *pGetActuatorsResponse = PyObject_CallFunction(pPluginGetActuatorsFunction, NULL);
    if (!pGetActuatorsResponse) {
        printCpp("Could not call get_actuator_list function on EPS derived class, make sure it is defined and error free");
        return 1;
    }
    if (PyList_Check(pGetActuatorsResponse)) {
        for (Py_ssize_t i = 0; i < PyList_Size(pGetActuatorsResponse); ++i) {
            printCpp("Processing return value from \"" + moduleName + "." + className + "." + getActuatorsFunctionName + "; item #" + std::to_string(i));
            PyObject *next = PyList_GetItem(pGetActuatorsResponse, i);
            std::string thisString = "";
            try {
                // TODO: Same note as above, the error is not being caught properly
                thisString = PyUnicode_AsUTF8(next);
            } catch (...) {
                printCpp("Invalid string return value from \"" + moduleName + "." + className + "." + getActuatorsFunctionName + "; item #" + std::to_string(i));
                return 1;
            }
            // check thisString for NULL due to bad response from the Python function
            thisPlugin.actuatorIDs.push_back(thisString);
            // Py_DECREF(next);  // PyList_GetItem returns a borrowed reference, DO NOT decrement!
        }
    } else {
        printCpp("Invalid return from get_actuator_list on class \"" + moduleName + "." + className + ", make sure it returns a list of strings");
        return 1;
    }
    Py_DECREF(pPluginGetActuatorsFunction);  // PyObject_GetAttrString returns a new reference, decrement it
    Py_DECREF(pGetActuatorsResponse);  // PyObject_CallFunction returns a new reference, decrement it

    // update the rest of the plugin call instance and store it
    thisPlugin.stringIdentifier = moduleName + "." + className;
    thisPlugin.pPluginMainFunction = pPluginMainFunction;
    thisPlugin.pPluginUpdateSensorFunction = pPluginUpdateSensorFunction;
    thisPlugin.callingPoint = thisCallingPoint;
    pluginInstances.push_back(thisPlugin);

    // hooray!
    return 0;
}

int PluginManager::callPluginInstances(CallingPoint callingPoint, SensedVariables &sensors,
                                       ActuatedVariables &actuators)
{
    for (auto & thisCall : this->pluginInstances) {
        if (thisCall.callingPoint != callingPoint) {
            // TODO: store the instances by type so we don't have to loop here each time
            continue;
        }
        // update the sensor values, this will be wired up nicer in E+
        for (auto sensorId : thisCall.sensorIDs) {
            double valueToPass;
            if (sensorId == "zoneOneTemperature") {
                valueToPass = sensors.zoneOneTemperature;
            } else if (sensorId == "zoneTwoTemperature") {
                valueToPass = sensors.zoneTwoTemperature;
            } else if (sensorId == "initialCoilSize") {
                valueToPass = sensors.initialCoilSize;
            } else if (sensorId == "PCUD_Tin") {
                valueToPass = sensors.PCUD_Tin;
            } else if (sensorId == "Qdot") {
                valueToPass = sensors.Qdot;
            } else {
                printCpp("Bad sensor value entered, did not find it available for this simulation: \"" + sensorId + "\"");
                return 1;
            }
            PyObject *pUpdateFunctionResponse = PyObject_CallFunction(thisCall.pPluginUpdateSensorFunction, "(sd)", sensorId.c_str(), valueToPass);
            if (!pUpdateFunctionResponse) {
                printCpp("Could not call to update sensor; sensor name = \"" + sensorId + "\", class = \"" + thisCall.stringIdentifier + "\"");
                return 1;
            }
            Py_DECREF(pUpdateFunctionResponse);  // PyObject_CallFunction returns new reference, decrement
        }
        // then call the main function
        PyObject *pFunctionResponse = PyObject_CallFunction(thisCall.pPluginMainFunction, NULL);
        if (!pFunctionResponse) {
            PyErr_Print();
            printCpp("Call to " + thisCall.stringIdentifier + " failed!");
            return 1;
        }
        if (PyList_Check(pFunctionResponse)) {
            for (Py_ssize_t i = 0; i < PyList_Size(pFunctionResponse); ++i) {
                PyObject* next = PyList_GetItem(pFunctionResponse, i);
                // do something with next
                double actuatedValue = PyFloat_AsDouble(next);
                // check actuatedValue for NULL due to bad response from the Python function
                std::string thisActuatorString = thisCall.actuatorIDs[(int)i];
                printCpp("Attempting to store value " + std::to_string(actuatedValue) + " into actuator: " + thisActuatorString);
                if (thisActuatorString == "updatedCoilSize") {
                    actuators.updatedCoilSize = actuatedValue;
                    printCpp("Updated Coil Size = " + std::to_string(actuators.updatedCoilSize));
                } else if (thisActuatorString == "zoneOneDamperPosition") {
                    actuators.zoneOneDamperPosition = actuatedValue;
                    printCpp("Updated Zone One Damper Position = " + std::to_string(actuators.zoneOneDamperPosition));
                } else if (thisActuatorString == "zoneTwoDamperPosition") {
                    actuators.zoneTwoDamperPosition = actuatedValue;
                    printCpp("Updated Zone Two Damper Position = " + std::to_string(actuators.zoneTwoDamperPosition));
                } else if (thisActuatorString == "PCUD_Tout") {
                    actuators.PCUD_Tout = actuatedValue;
                    printCpp("Updated PCUD Outlet Temp = " + std::to_string(actuators.PCUD_Tout));
                } else if (thisActuatorString == "PCUD_Mdot_Request") {
                    actuators.PCUD_Mdot_Request = actuatedValue;
                    printCpp("Updated PCUD Mdot = " + std::to_string(actuators.PCUD_Mdot_Request));
                }
                // Py_DECREF(next);  // PyList_GetItem returns a borrowed reference, do not decrement
            }
        } else {
            printCpp("Invalid return from main() on class \"" + thisCall.stringIdentifier + ", make sure it returns a list of actuated float values");
            return 1;
        }
        Py_DECREF(pFunctionResponse);  // PyObject_CallFunction returns new reference, decrement
    }
    return 0;  // wait til we're all done
}

int PluginManager::addToPythonPath(std::string path) {
    std::string command = "sys.path.insert(0, \"" + path + "\")";
    if (PyRun_SimpleString(command.c_str()) == 0) {
        printCpp("Successfully added path \"" + path + "\" to the sys.path in Python");
        PyRun_SimpleString("print(' EPS : ' + str(sys.path))");
        return 0;
    } else {
        printCpp("ERROR adding \"" + path + "\" to the sys.path in Python");
        return 1;
    }
}
