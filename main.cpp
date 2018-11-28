#include <Python.h>
#include <vector>

enum class CallingPoints {  // this would need to get exposed to EMS program writers if calling_point is an argument
    AFTER_SIZING = 0,
    HVAC_TIME_STEP_LOOP = 1
};
struct EMSCallingPoint {
    PyObject *pModule;
    PyObject *pEMSMainFunction;
    CallingPoints callingPoint;
};
std::vector<struct EMSCallingPoint> emsCallingPoints;

int initPyMSInstances(const char* fileName, const char* functionName, CallingPoints callingPoint) {
    // initialize Python
    Py_Initialize();
    // load the EMS file
    PyObject *pName = PyUnicode_DecodeFSDefault(fileName);
    /* Error checking of pName left out */

    EMSCallingPoint thisEMSCall;
    thisEMSCall.callingPoint = callingPoint;
    // get a reference to the entire EMS module
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);pName;
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

int closePyEMS(EMSCallingPoint thisCall) {
    Py_DECREF(thisCall.pModule);
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
}

int callEMSInstance(CallingPoints callingPoint, EMSCallingPoint thisCall)
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
    if (pValue != NULL) {
        printf("Result of call: %ld\n", PyLong_AsLong(pValue));
        Py_DECREF(pValue);
    } else {
        Py_DECREF(thisCall.pEMSMainFunction);
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
    if (initPyMSInstances("ems_implementation", "main", CallingPoints::HVAC_TIME_STEP_LOOP) != 0) {
        return 1;
    }
    fprintf(stdout, "Performing Sizing\n");
    callEMSInstance(CallingPoints::AFTER_SIZING, emsCallingPoints[0]);
    fprintf(stdout, "Inside HVAC TimeStep Loop\n");
    callEMSInstance(CallingPoints::HVAC_TIME_STEP_LOOP, emsCallingPoints[0]);
    callEMSInstance(CallingPoints::HVAC_TIME_STEP_LOOP, emsCallingPoints[0]);
    callEMSInstance(CallingPoints::HVAC_TIME_STEP_LOOP, emsCallingPoints[0]);
    fprintf(stdout, "Timesteps Complete\n");
    closePyEMS(emsCallingPoints[0]);
    fprintf(stdout, "EnergyPlus Complete\n");
}
