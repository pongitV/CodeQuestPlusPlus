#include "Logger.h"

namespace {
    std::mutex logMutex;

    const char* levelToString(Logger::Level level) {
        switch (level) {
            case Logger::Level::Debug: return "[DEBUG]";
            case Logger::Level::Info: return "[INFO]";
            case Logger::Level::Warning: return "[WARN]";
            case Logger::Level::Error: return "[ERR]";
            default: return "[LOG]";
        }
    }
}

void Logger::log(Level level, const std::string& message, const std::string& source) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << levelToString(level);
    if (!source.empty()) {
        std::cout << " [" << source << "]";
    }
    std::cout << " " << message << "\n";
}
