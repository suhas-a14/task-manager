#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <ctime>

// Initialize static members
std::ofstream Logger::logFile;
std::string Logger::logFilePath = "wintask_monitor.log";

void Logger::SetLogFile(const std::string& filepath) {
    if (logFile.is_open()) {
        logFile.close();
    }
    logFilePath = filepath;
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::string timestamp = GetCurrentTimestamp();
    std::string levelStr = LogLevelToString(level);
    std::string logLine = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Print critical errors to standard error stream
    if (level == LogLevel::ERR) {
        std::cerr << logLine << std::endl;
    }
    
    // Open file stream in append mode if not already open
    if (!logFile.is_open()) {
        logFile.open(logFilePath, std::ios::app);
    }
    
    if (logFile.is_open()) {
        logFile << logLine << std::endl;
    }
}

std::string Logger::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERR:     return "ERROR";
        default:                return "UNKNOWN";
    }
}

std::string Logger::GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    // Using standard std::localtime, which is portable across MSVC and MinGW GCC.
    // Thread safety is not an issue since this application is entirely single-threaded.
    std::tm* bt = std::localtime(&in_time_t);
    if (!bt) {
        return "0000-00-00 00:00:00";
    }
    
    std::stringstream ss;
    ss << std::put_time(bt, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
