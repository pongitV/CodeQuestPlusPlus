#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <mutex>

class Logger {
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    static void log(Level level, const std::string& message, const std::string& source = "");
    
    template<typename T>
    static void logValue(Level level, const std::string& variableName, T value, const std::string& source = "Performance") {
        log(level, variableName + " = " + std::to_string(value), source);
    }
};
