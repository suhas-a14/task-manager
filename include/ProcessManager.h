#pragma once
#include "ProcessInfo.h"
#include <vector>
#include <string>

class ProcessManager {
public:
    // Enumerates all processes running in the system using Windows APIs
    std::vector<ProcessInfo> EnumerateProcesses();

    // Filters process list by matching names (case-insensitive substring match)
    std::vector<ProcessInfo> FilterProcessesByName(const std::vector<ProcessInfo>& processes, const std::string& searchStr);

    // Sorting routines using standard std::sort
    static void SortByPID(std::vector<ProcessInfo>& processes, bool ascending = true);
    static void SortByName(std::vector<ProcessInfo>& processes, bool ascending = true);
    static void SortByMemory(std::vector<ProcessInfo>& processes, bool ascending = true);

    // Retrieves detailed information for a specific PID. Returns false if not found.
    bool GetProcessDetails(unsigned long pid, ProcessInfo& outDetails);

    // Safely attempts to terminate a process by PID. Returns true on success, else sets outErrorMsg.
    bool TerminateProcessByPID(unsigned long pid, std::string& outErrorMsg);
};
