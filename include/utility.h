#ifndef ENERGYPLUSPYEMS_UTILITY_H
#define ENERGYPLUSPYEMS_UTILITY_H

#include <stdexcept>
#include <string>

void printCpp(std::string message);

std::string sanitizedPath(std::string path);

class FatalError : public std::exception {
private:
    std::string m_error;
public:
    FatalError(std::string error) : m_error(error) {}
    const char* what() const noexcept { return m_error.c_str(); } // C++11 version
};

#endif //ENERGYPLUSPYEMS_UTILITY_H
