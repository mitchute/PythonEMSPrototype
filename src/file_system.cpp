// borrowed from the E+ code

#include <string>

#ifdef _WIN32
#include <Shlwapi.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <file_system.h>
#include <utility.h>

#ifdef _WIN32
char const pathChar('\\');
    char const altpathChar('/');
#elif __linux__
char const pathChar('/');
char const altpathChar('\\');
#elif __unix__
char const pathChar('/');
    char const altpathChar('\\');
#elif __posix__
    char const pathChar('/');
    char const altpathChar('\\');
#elif __APPLE__
    char const pathChar('/');
    char const altpathChar('\\');
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

std::string getProgramPath()
{
    char executableRelativePath[1024];

#ifdef __APPLE__
    uint32_t pathSize = sizeof(executableRelativePath);
        _NSGetExecutablePath(executableRelativePath, &pathSize);
#elif __linux__
    ssize_t len = readlink("/proc/self/exe", executableRelativePath, sizeof(executableRelativePath) - 1);
    if (len == -1) {
        printCpp("ERROR: Unable to locate executable.");
        std::exit(EXIT_FAILURE);
    } else {
        executableRelativePath[len] = '\0';
    }
#elif _WIN32
    GetModuleFileName(NULL, executableRelativePath, sizeof(executableRelativePath));
#endif

    return std::string(executableRelativePath);
}

std::string getParentDirectoryPath(std::string const &path)
{
    std::string tempPath = path;
    if (path.at(path.size() - 1) == pathChar) tempPath = path.substr(0, path.size() - 1);

    size_t pathCharPosition = tempPath.find_last_of(pathChar);
    tempPath = tempPath.substr(0, pathCharPosition + 1);

    if (tempPath.empty()) tempPath = ".";

    return tempPath;
}
