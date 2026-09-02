#pragma once
#include <string>
#include <fstream>

enum class LogLevel {
    INFO,
    WARNING,
    ERR
};

class Logger {
public:
    // Log a message with a specific severity level
    static void Log(LogLevel level, const std::string& message);
    
    // Set the path to the file where log entries are written
    static void SetLogFile(const std::string& filepath);

private:
    // Convert LogLevel enum to string representation
    static std::string LogLevelToString(LogLevel level);
    
    // Helper to get formatted current timestamp [YYYY-MM-DD HH:MM:SS]
    static std::string GetCurrentTimestamp();

    static std::ofstream logFile;
    static std::string logFilePath;
};
