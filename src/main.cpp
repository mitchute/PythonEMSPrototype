#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <ems_manager.h>
#include <utility.h>

int
main(int argc, char *argv[])
{
    // start up E+ and create an EMS manager to store instances, etc.
    printCpp("Starting (Fake) EnergyPlus");
    EMSManager eplusEmsManager;

    // read input file, add new PyEMS for each line found: lines should be `module,class_name'
    if (argc != 2) {
        std::cout << "Should call this with a single argument: the path to the (fake) idf file\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            std::string moduleName = line.substr(0, line.find(","));
            std::string className = line.substr(line.find(",") + 1, line.length());
            if (eplusEmsManager.initPyEMSInstanceFromClass(moduleName, className) != 0) {
                return 1;
            }
        }
        file.close();
    } else {
        std::cout << "Could not find in.idf!\n";
        return 1;
    }

    // "run" EnergyPlus, mimicking some big ticket items along the way
    SensedVariables sensedData;
    ActuatedVariables actuatedData;
    printCpp("Performing Sizing");
    sensedData.initialCoilSize = 1108.73;
    if (eplusEmsManager.callEMSInstances(CallingPoint::AFTER_SIZING, sensedData, actuatedData) != 0) return 1;
    printCpp("Inside HVAC TimeStep Loop");
    sensedData.zoneOneTemperature = 23.4;
    sensedData.zoneTwoTemperature = 24.3;
    if (eplusEmsManager.callEMSInstances(CallingPoint::HVAC_TIME_STEP_LOOP, sensedData, actuatedData) != 0) return 1;
    printCpp("Inside HVAC TimeStep Loop");
    sensedData.zoneOneTemperature = 22.4;
    sensedData.zoneTwoTemperature = 25.3;
    if (eplusEmsManager.callEMSInstances(CallingPoint::HVAC_TIME_STEP_LOOP, sensedData, actuatedData) != 0) return 1;
    printCpp("Inside HVAC TimeStep Loop");
    sensedData.zoneOneTemperature = 20.6;
    sensedData.zoneTwoTemperature = 27.3;
    if (eplusEmsManager.callEMSInstances(CallingPoint::HVAC_TIME_STEP_LOOP, sensedData, actuatedData) != 0) return 1;
    printCpp("Timesteps Complete");
    printCpp("EnergyPlus Complete");
}
