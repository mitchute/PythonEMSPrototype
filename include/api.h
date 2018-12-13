#ifndef ENERGYPLUSPYEMS_API_H
#define ENERGYPLUSPYEMS_API_H

extern "C" {
double saturationPressureFunctionOfTemperature(double temperatureC);
bool eplusFatalHalt(char *message);
bool eplusSevereError(char *message);
bool eplusWarning(char *message);
}

#endif //ENERGYPLUSPYEMS_API_H
