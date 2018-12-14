#ifndef ENERGYPLUSPYEMS_API_H
#define ENERGYPLUSPYEMS_API_H

#if _WIN32 || _MSC_VER
#define FAKE_EPLUS_API __declspec(dllimport)
#else
#define FAKE_EPLUS_API
#endif

extern "C" {
double FAKE_EPLUS_API saturationPressureFunctionOfTemperature(double temperatureC);
void FAKE_EPLUS_API eplusFatalHalt(char *message);
void FAKE_EPLUS_API eplusSevereError(char *message);
void FAKE_EPLUS_API eplusWarning(char *message);
bool FAKE_EPLUS_API isFatalTriggered();
const char* FAKE_EPLUS_API getFatalMessage();
}

#endif //ENERGYPLUSPYEMS_API_H
