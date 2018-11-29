#include <string>
#include <vector>

#include <Python.h>

#include <ems_manager.h>
#include <utility.h>

int EMSManager::initPyMSInstances(const char* fileName, const char* functionName, CallingPoints callingPoint) {
    // initialize Python
    Py_NoSiteFlag = 1;
    Py_Initialize();
    // Py_SetProgramName(argv[0]);
    // Py_SetPythonHome('.');
    // Py_InitializeEx(0);

    // load the EMS file
    PyObject *pName = PyUnicode_DecodeFSDefault(fileName);
    /* Error checking of pName left out */

    EMSCallingPoint thisEMSCall;
    thisEMSCall.callingPoint = callingPoint;
    // get a reference to the entire EMS module
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule != NULL) {
        thisEMSCall.pModule = pModule;
        // get a reference to the EMS function
        PyObject *pEMSMainFunction = PyObject_GetAttrString(pModule, functionName);
        /* pFunc is a new reference */
        if (!pEMSMainFunction || !PyCallable_Check(pEMSMainFunction)) {
            if (PyErr_Occurred()) {
                PyErr_Print();
            }
            fprintf(stderr, "Cannot find function \"%s\"\n", "main");
            return 1;
        }
        thisEMSCall.pEMSMainFunction = pEMSMainFunction;
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "ems_implementation");
        return 1;
    }
    emsCallingPoints.push_back(thisEMSCall);
    return 0;
}

int EMSManager::closePyEMS(EMSCallingPoint thisCall) {
    Py_DECREF(thisCall.pModule);
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    return 0;
}

int EMSManager::callEMSInstance(CallingPoints callingPoint, EMSCallingPoint thisCall)
{

    if (thisCall.callingPoint != callingPoint) {
        // this call isn't for now
        return 0;
    }

    // build out a tuple of values to pass into the EMS function
    PyObject *pEMSArgs = PyTuple_New(1);
    PyObject *pValue = PyLong_FromLong(2);
    PyTuple_SetItem(pEMSArgs, 0, pValue);

    // call the EMS function
    Py_INCREF(thisCall.pEMSMainFunction);
    pValue = PyObject_CallObject(thisCall.pEMSMainFunction, pEMSArgs);

    Py_DECREF(pEMSArgs);
    if (pValue) {
        Py_DECREF(pValue);
    } else {
        Py_DECREF(thisCall.pEMSMainFunction);
        PyErr_Print();
        print_cpp("Call failed!");
        return 1;
    }

    return 0;
}
