#include <cmath>
#include <iostream>

#include <api.h>

extern "C" {
double saturationPressureFunctionOfTemperature(double temperatureC) {
    // returns Pascals
    double const tempKelvin = temperatureC + 273.15;
    if (tempKelvin < 173.15) {
        return 0.0017;
    } else if (tempKelvin < 273.15) {
        double const c1 = (-5674.5359);
        double const c2(6.3925247);
        double const c3(-0.9677843e-2);
        double const c4(0.62215701e-6);
        double const c5(0.20747825e-8);
        double const c6(-0.9484024e-12);
        double const c7(4.1635019);
        return std::exp(
                c1 / tempKelvin + c2 + tempKelvin * (c3 + tempKelvin * (c4 + tempKelvin * (c5 + c6 * tempKelvin))) +
                c7 * std::log(tempKelvin));
    } else if (tempKelvin <= 473.15) {
        double const c8(-5800.2206);
        double const c9(1.3914993);
        double const c10(-0.048640239);
        double const c11(0.41764768e-4);
        double const c12(-0.14452093e-7);
        double const c13(6.5459673);
        return std::exp(c8 / tempKelvin + c9 + tempKelvin * (c10 + tempKelvin * (c11 + tempKelvin * c12)) +
                        c13 * std::log(tempKelvin));
    } else {
        return 1555000.0;
    }
}

bool pluginTriggeredFatalError = false;
std::string pluginTriggeredFatalMessage = "";

void eplusFatalHalt(char* message) {
    std::cout << "FATAL ERROR: " << message << std::endl;
    pluginTriggeredFatalError = true;
    pluginTriggeredFatalMessage = message;
}

void eplusSevereError(char* message) {
    std::cout << "SEVERE Error: " << message << std::endl;
}

void eplusWarning(char* message) {
    std::cout << "Warning: " << message << std::endl;
}

bool isFatalTriggered() {
    return pluginTriggeredFatalError;
}
const char* getFatalMessage() {
    return pluginTriggeredFatalMessage.c_str();
}
}
