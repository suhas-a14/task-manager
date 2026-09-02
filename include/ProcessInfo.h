#pragma once
#include <string>

struct ProcessInfo {
    unsigned long pid = 0;            // Process Identifier
    std::string name;                 // Executable name of the process (e.g., "explorer.exe")
    unsigned long long memoryUsage = 0; // Memory usage in bytes (Working Set size)
    std::string status = "Unknown";   // Human readable process status (e.g. "Running", "Access Denied")
    unsigned long handleCount = 0;    // Number of handles opened by the process
};
