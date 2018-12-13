#include <iostream>
#include <string>

#include <utility.h>

void printCpp(std::string message) {
    std::cout << " C++ : " << message << std::endl;
}

std::string sanitizedPath(std::string path) {
    // there are parts of this program that need to write out a string to execute in Python
    // because of that, escaped backslashes actually need double escaping
    // plus, the string cannot end with a backslash
    // sanitize the path to remove any trailing backslash
    if (path.substr(path.length()-1, path.length()) == "\\") {
        path = path.substr(0, path.length()-1);
    }
    // then sanitize it to escape the backslashes for writing the string literal to Python
    std::string sanitizedDir = "";
    for(int i=0; i<path.size(); i++) {
        if (path[i] == '\\') {
            sanitizedDir += "\\\\";
        } else {
            sanitizedDir += path[i];
        }
    }
    return sanitizedDir;
}

bool pluginTriggeredFatalError = false;
std::string pluginTriggeredFatalMessage = "";

void setFatalTriggered(std::string message) {
    pluginTriggeredFatalError = true;
    pluginTriggeredFatalMessage = message;
}

bool isFatalTriggered() {
    return pluginTriggeredFatalError;
}

std::string fatalMessage() {
    return pluginTriggeredFatalMessage;
}