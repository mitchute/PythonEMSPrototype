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

std::string const mainFunctionName = "ems_main";
std::string const callingPointFunctionName = "get_calling_point";
std::string const getSensorsFunctionName = "get_sensed_data_list";
std::string const getActuatorsFunctionName = "get_actuator_list";
std::string const updateSensorFunctionName = "update_sensed_datum";

int EMSManager::initPyEMSInstanceFromClass(std::string moduleName, std::string className) {

    PyEMSInstance thisEMSCall;

    // this first section is really all about just ultimately getting a full Python class instance
    // this answer helped with a few things: https://ru.stackoverflow.com/a/785927
    PyObject *pModuleName = PyUnicode_DecodeFSDefault(moduleName.c_str());
    PyObject *pModule = PyImport_Import(pModuleName);
    Py_DECREF(pModuleName);
    if (!pModule) {
        printCpp("Failed to import module \"" + moduleName + "\"");
        return 1;
    }
    PyObject *pModuleDict = PyModule_GetDict(pModule);
    Py_DECREF(pModule);
    if (!pModuleDict) {
        printCpp("Failed to read module dictionary from module \"" + moduleName + "\"");
        return 1;
    }
    PyObject *pClass = PyDict_GetItemString(pModuleDict, className.c_str());
    Py_DECREF(pModuleDict);
    if (!pClass) {
        printCpp("Failed to get class type \"" + className + "\" from module \"" + moduleName + "\"");
        return 1;
    }
    if (!PyCallable_Check(pClass)) {
        printCpp("Got class type \"" + className + "\", but it cannot be called/instantiated");
        return 1;
    }
    PyObject *pClassInstance = PyObject_CallObject(pClass, NULL);
    Py_DECREF(pClass);
    if (!pClassInstance) {
        printCpp("Something went awry calling class constructor for class \"" + className + "\"");
        return 1;
    }
    // the only thing that we haven't Py_DECREF'd in the above block is pClassInstance since we need it below

    // now grab the function pointers to each of the relevant functions
    PyObject *pEMSMainFunction = PyObject_GetAttrString(pClassInstance, mainFunctionName.c_str());
    if (!pEMSMainFunction || !PyCallable_Check(pEMSMainFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + mainFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pEMSGetSensorsFunction = PyObject_GetAttrString(pClassInstance, getSensorsFunctionName.c_str());
    if (!pEMSGetSensorsFunction || !PyCallable_Check(pEMSGetSensorsFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + getSensorsFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pEMSUpdateSensorFunction = PyObject_GetAttrString(pClassInstance, updateSensorFunctionName.c_str());
    if (!pEMSUpdateSensorFunction || !PyCallable_Check(pEMSUpdateSensorFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + updateSensorFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pEMSCallingPointFunction = PyObject_GetAttrString(pClassInstance, callingPointFunctionName.c_str());
    if (!pEMSCallingPointFunction || !PyCallable_Check(pEMSCallingPointFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + callingPointFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }
    PyObject *pEMSGetActuatorsFunction = PyObject_GetAttrString(pClassInstance, getActuatorsFunctionName.c_str());
    if (!pEMSGetActuatorsFunction || !PyCallable_Check(pEMSGetActuatorsFunction)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        printCpp("Could not find function \"" + getActuatorsFunctionName + "\" on class \"" + moduleName + "." + className + "\"");
        return 1;
    }

    // we will call to get the calling point for this EMS class, then we're done with that function
    PyObject *pCallingPointResponse = PyObject_CallFunction(pEMSCallingPointFunction, NULL);
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
    Py_DECREF(pEMSCallingPointFunction);
    Py_DECREF(pCallingPointResponse);

    // we also call to get the list of sensors, then we're done with that function
    PyObject *pGetSensorsResponse = PyObject_CallFunction(pEMSGetSensorsFunction, NULL);
    if (!pGetSensorsResponse) {
        printCpp("Could not call get_sensed_data_list function on EMS derived class, make sure it is defined and error free");
        return 1;
    }
    if (PyList_Check(pGetSensorsResponse)) {
        for (Py_ssize_t i = 0; i < PyList_Size(pGetSensorsResponse); ++i) {
            printCpp("Processing return value from \"" + moduleName + "." + className + "." + getSensorsFunctionName + "; item #" + std::to_string(i));
            PyObject *next = PyList_GetItem(pGetSensorsResponse, i);
            std::string thisString = "";
            try {
                // TODO: Figure out why this isn't caught.
                // It throws a std::logic_error if you change the return value of a get_sensed_data_list to non-strings, like [2.3, 2.1]
                // But wrapping it in a try block doesn't actually catch it.  I read online it could be due to a difference in compiler from
                // what I am currently using to compile this C++ and what they used to compile the underlying Python library
                thisString = PyUnicode_AsUTF8(next);
            } catch (...) {
                printCpp("Invalid string return value from \"" + moduleName + "." + className + "." + getSensorsFunctionName + "; item #" + std::to_string(i));
                return 1;
            }
            thisEMSCall.sensorIDs.push_back(thisString);
            // calling the update_sensor function - it will verify functionality and also initialize the dict
            PyObject *pUpdateFunctionResponse = PyObject_CallFunction(pEMSUpdateSensorFunction, "(sd)", thisString.c_str(), 0.0);
            if (!pUpdateFunctionResponse) {
                printCpp("Could not call to update sensor data function, this is a base class function so either it's improperly overridden or EMS class does not inherit EMS base");
                return 1;
            }
            Py_DECREF(next);
            Py_DECREF(pUpdateFunctionResponse);
        }
    } else {
        printCpp("Invalid return from get_sensed_data_list on class \"" + moduleName + "." + className + ", make sure it returns a list of strings");
        return 1;
    }
    Py_DECREF(pEMSGetSensorsFunction);
    Py_DECREF(pGetSensorsResponse);

    // finally we also call to get the list of actuators, and we're done with that function also
    PyObject *pGetActuatorsResponse = PyObject_CallFunction(pEMSGetActuatorsFunction, NULL);
    if (!pGetActuatorsResponse) {
        printCpp("Could not call get_actuator_list function on EMS derived class, make sure it is defined and error free");
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
            thisEMSCall.actuatorIDs.push_back(thisString);
            Py_DECREF(next);
        }
    } else {
        printCpp("Invalid return from get_actuator_list on class \"" + moduleName + "." + className + ", make sure it returns a list of strings");
        return 1;
    }
    Py_DECREF(pEMSGetActuatorsFunction);
    Py_DECREF(pGetActuatorsResponse);

    // update the rest of the EMS call instance and store it
    thisEMSCall.stringIdentifier = moduleName + "." + className;
    thisEMSCall.pEMSMainFunction = pEMSMainFunction;
    thisEMSCall.pEMSUpdateSensorFunction = pEMSUpdateSensorFunction;
    thisEMSCall.callingPoint = thisCallingPoint;
    pyEMSInstances.push_back(thisEMSCall);

    // hooray!
    return 0;
}

int EMSManager::callEMSInstances(CallingPoint callingPoint, SensedVariables &sensors, ActuatedVariables &actuators)
{
    for (auto & thisCall : this->pyEMSInstances) {
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
            } else {
                printCpp("Bad sensor value entered, did not find it available for this simulation: \"" + sensorId + "\"");
                return 1;
            }
            PyObject *pUpdateFunctionResponse = PyObject_CallFunction(thisCall.pEMSUpdateSensorFunction, "(sd)", sensorId.c_str(), valueToPass);
            if (!pUpdateFunctionResponse) {
                printCpp("Could not call to update sensor; sensor name = \"" + sensorId + "\", class = \"" + thisCall.stringIdentifier + "\"");
                return 1;
            }
            Py_DECREF(pUpdateFunctionResponse);
        }
        // then call the main function
        PyObject *pFunctionResponse = PyObject_CallFunction(thisCall.pEMSMainFunction, NULL);
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
                }
                Py_DECREF(next);
            }
        } else {
            printCpp("Invalid return from ems_main on class \"" + thisCall.stringIdentifier + ", make sure it returns a list of actuated float values");
            return 1;
        }
        Py_DECREF(pFunctionResponse);
    }
    return 0;  // wait til we're all done
}

int EMSManager::addToPythonPath(std::string path) {
    std::string command = "sys.path.insert(0, \"" + path + "\")";
    if (PyRun_SimpleString(command.c_str()) == 0) {
        printCpp("Successfully added path \"" + path + "\" to the sys.path in Python");
        PyRun_SimpleString("print(sys.path)");
        return 0;
    } else {
        printCpp("ERROR adding \"" + path + "\" to the sys.path in Python");
        return 1;
    }
}
