#include <iostream>
#include <string>
#include <vector>

#include <ems_manager.h>
#include <utility.h>

int
main(int argc, char *argv[])
{
    print_cpp("Starting (Fake) EnergyPlus");
    EMSManager eplus_ems_manager;
    if (eplus_ems_manager.initPyMSInstances("ems_implementation", "main", CallingPoints::HVAC_TIME_STEP_LOOP) != 0) {
        return 1;
    }
    print_cpp("Performing Sizing");
    eplus_ems_manager.callEMSInstance(CallingPoints::AFTER_SIZING, eplus_ems_manager.emsCallingPoints[0]);
    print_cpp("Inside HVAC TimeStep Loop");
    eplus_ems_manager.callEMSInstance(CallingPoints::HVAC_TIME_STEP_LOOP, eplus_ems_manager.emsCallingPoints[0]);
    eplus_ems_manager.callEMSInstance(CallingPoints::HVAC_TIME_STEP_LOOP, eplus_ems_manager.emsCallingPoints[0]);
    eplus_ems_manager.callEMSInstance(CallingPoints::HVAC_TIME_STEP_LOOP, eplus_ems_manager.emsCallingPoints[0]);
    print_cpp("Timesteps Complete");
    eplus_ems_manager.closePyEMS(eplus_ems_manager.emsCallingPoints[0]);
    print_cpp("EnergyPlus Complete");
}
