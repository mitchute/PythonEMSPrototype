#ifndef ENERGYPLUSPYEMS_API_H
#define ENERGYPLUSPYEMS_API_H

extern "C" {
double saturationPressureFunctionOfTemperature(double temperatureC);
void eplusFatalHalt(char *message);
void eplusSevereError(char *message);
void eplusWarning(char *message);
bool isFatalTriggered();
const char* getFatalMessage();
}

#endif //ENERGYPLUSPYEMS_API_H
