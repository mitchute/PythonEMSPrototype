#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <ems_manager.h>
#include <file_system.h>
#include <utility.h>

int
main(int argc, char *argv[])
{
    // start up E+ and create a plugin manager to store instances, etc.
    printCpp("Starting (Fake) EnergyPlus");
    PluginManager eplusPluginManager;

    // we need to figure out the right way to find the path to the binary itself
    // we want to point Python to a folder containing a pyms folder in it
    // for the case of this prototype, it's the root of the repo
    // for the case of an E+ distribution, it will be probably the root of the E+ install
    // I think this is already available from the command line interface work
    eplusPluginManager.addToPythonPath(".");

    // so I think we'll want to add to the front of path, in this order:
    // - the program executable parent directory, so that it could find the installed pyms,
    // - the current working directory, so if they have pyms installed locally it will find that one first
    // - any additional paths they ask for in the input file
    std::string programPath = getProgramPath();
    std::string programDir = getParentDirectoryPath(programPath);
    printCpp("Getting program path: " + programPath);
    printCpp("Getting program dir: " + programDir);
    std::string sanitizedDir = sanitizedPath(programDir);
    eplusPluginManager.addToPythonPath(sanitizedDir);

    // read input file
    if (argc != 2) {
        std::cout << "Should call this with a single argument: the path to the (fake) idf file\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            if (line.substr(0, 1) == "!") {
                continue;
            }
            std::string remainingLine = line;
            // find the input type (dir or class) token
            size_t comma_position = remainingLine.find(",");
            if (comma_position == std::string::npos) {
                printCpp("Bad line in input file, could not find first comma");
                return 1;
            }
            std::string objectType = remainingLine.substr(0, comma_position);
            remainingLine = remainingLine.substr(comma_position + 1, remainingLine.length());
            // then process the rest of the line based on this first token
            if (objectType == "dir") {
                // need to parse the python dir and add it to the search path
                std::string searchDir = remainingLine;
                sanitizedDir = sanitizedPath(searchDir);
                if (eplusPluginManager.addToPythonPath(sanitizedDir) != 0) {
                    return 1;
                }
            } else if (objectType == "class") {
                std::string moduleName = remainingLine.substr(0, remainingLine.find(","));
                std::string className = remainingLine.substr(remainingLine.find(",") + 1, remainingLine.length());
                if (eplusPluginManager.initPluginInstanceFromClass(moduleName, className) != 0) {
                    return 1;
                }
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
    if (eplusPluginManager.callPluginInstances(CallingPoint::AFTER_SIZING, sensedData, actuatedData) != 0) return 1;
    printCpp("Inside HVAC TimeStep Loop");
    sensedData.zoneOneTemperature = 23.4;
    sensedData.zoneTwoTemperature = 24.3;    
    if (eplusPluginManager.callPluginInstances(CallingPoint::HVAC_TIME_STEP_LOOP, sensedData, actuatedData) != 0) return 1;
    printCpp("User Defined Component Model");
    sensedData.PCUD_Tin = 80;
    sensedData.Qdot = 0.0;  
    if (eplusPluginManager.callPluginInstances(CallingPoint::User_Defined_Component_Model, sensedData, actuatedData) != 0) return 1;    
    printCpp("Inside HVAC TimeStep Loop");
    sensedData.zoneOneTemperature = 22.4;
    sensedData.zoneTwoTemperature = 25.3;
    if (eplusPluginManager.callPluginInstances(CallingPoint::HVAC_TIME_STEP_LOOP, sensedData, actuatedData) != 0) return 1;
    printCpp("User Defined Component Model");
    sensedData.PCUD_Tin = 80;
    sensedData.Qdot = 1.0;  
    if (eplusPluginManager.callPluginInstances(CallingPoint::User_Defined_Component_Model, sensedData, actuatedData) != 0) return 1;    
    printCpp("Inside HVAC TimeStep Loop");
    sensedData.zoneOneTemperature = 20.6;
    sensedData.zoneTwoTemperature = 27.3;
    if (eplusPluginManager.callPluginInstances(CallingPoint::HVAC_TIME_STEP_LOOP, sensedData, actuatedData) != 0) return 1;
    printCpp("User Defined Component Model");
    sensedData.PCUD_Tin = 80;
    sensedData.Qdot = 2.0;  
    if (eplusPluginManager.callPluginInstances(CallingPoint::User_Defined_Component_Model, sensedData, actuatedData) != 0) return 1;    
    printCpp("Timesteps Complete");
    printCpp("EnergyPlus Complete");
}
