#ifndef ENERGYPLUSPYEMS_EMS_MANAGER_H
#define ENERGYPLUSPYEMS_EMS_MANAGER_H

#include <vector>

#include <Python.h>

enum class CallingPoints {  // hardwired because this would need to get exposed to EMS program writers if calling_point is an argument
    AFTER_SIZING = 0,
    HVAC_TIME_STEP_LOOP = 1
};

struct EMSCallingPoint {
    PyObject *pModule;
    PyObject *pEMSMainFunction;
    CallingPoints callingPoint;
};

class EMSManager {
public:
    std::vector<EMSCallingPoint> emsCallingPoints;
    int initPyMSInstances(const char* fileName, const char* functionName, CallingPoints callingPoint);
    int closePyEMS(EMSCallingPoint thisCall);
    int callEMSInstance(CallingPoints callingPoint, EMSCallingPoint thisCall);
};

#endif //ENERGYPLUSPYEMS_EMS_MANAGER_H
