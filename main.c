#include <Python.h>

PyObject *pModule, *pEMSMainFunction;

int initPyMSInstances() {
    // initialize Python
    Py_Initialize();
    // load the EMS file
    PyObject *pName = PyUnicode_DecodeFSDefault("ems_implementation");
    /* Error checking of pName left out */

    // get a reference to the entire EMS module
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);pName;
    if (pModule != NULL) {
        // get a reference to the EMS function
        pEMSMainFunction = PyObject_GetAttrString(pModule, "main");
        /* pFunc is a new reference */
        if (!pEMSMainFunction || !PyCallable_Check(pEMSMainFunction)) {
            if (PyErr_Occurred()) {
                PyErr_Print();
            }
            fprintf(stderr, "Cannot find function \"%s\"\n", "main");
            return 1;
        }
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "ems_implementation");
        return 1;
    }
}

int closePyEMS() {
    Py_DECREF(pModule);
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
}

int callEMSInstance(const char* callingPoint)
{
    // build out a tuple of values to pass into the EMS function
    PyObject *pCallingPoint = PyUnicode_DecodeFSDefault(callingPoint);

    PyObject *pEMSArgs = PyTuple_New(1);
    PyObject *pValue = PyLong_FromLong(2);
    PyTuple_SetItem(pEMSArgs, 0, pValue);

    PyObject *pAllArgs = PyTuple_New(2);
    PyTuple_SetItem(pAllArgs, 0, pCallingPoint);
    PyTuple_SetItem(pAllArgs, 1, pEMSArgs);

    // call the EMS function
    Py_INCREF(pEMSMainFunction);
    pValue = PyObject_CallObject(pEMSMainFunction, pAllArgs);

    Py_DECREF(pAllArgs);
    Py_DECREF(pEMSArgs);
    if (pValue != NULL) {
        printf("Result of call: %ld\n", PyLong_AsLong(pValue));
        Py_DECREF(pValue);
    } else {
        Py_DECREF(pEMSMainFunction);
        PyErr_Print();
        fprintf(stderr,"Call failed\n");
        return 1;
    }

    return 0;
}

int
main(int argc, char *argv[])
{
    fprintf(stdout, "Starting (Fake) EnergyPlus\n");
    initPyMSInstances();
    fprintf(stdout, "Performing Sizing\n");
    callEMSInstance("AfterSizing");
    fprintf(stdout, "Inside HVAC TimeStep Loop\n");
    callEMSInstance("HVACTimeStepLoop");
    callEMSInstance("HVACTimeStepLoop");
    callEMSInstance("HVACTimeStepLoop");
    fprintf(stdout, "Timesteps Complete\n");
    closePyEMS();
    fprintf(stdout, "EnergyPlus Complete\n");
}
