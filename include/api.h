#ifndef ENERGYPLUSPYEMS_API_H
#define ENERGYPLUSPYEMS_API_H

#if _WIN32 || _MSC_VER
#define FAKE_EPLUS_API __declspec( dllexport )
#else
#define FAKE_EPLUS_API
#endif

extern "C" {
FAKE_EPLUS_API double saturationPressureFunctionOfTemperature(double temperatureC);
FAKE_EPLUS_API void eplusFatalHalt(char *message);
FAKE_EPLUS_API void eplusSevereError(char *message);
FAKE_EPLUS_API void eplusWarning(char *message);
FAKE_EPLUS_API bool isFatalTriggered();
FAKE_EPLUS_API const char* getFatalMessage();
}

#endif //ENERGYPLUSPYEMS_API_H
