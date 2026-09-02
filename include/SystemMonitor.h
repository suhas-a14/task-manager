#pragma once
#include "CPUInfo.h"
#include "MemoryInfo.h"
#include "ProcessManager.h"
#include "ProcessInfo.h"
#include <vector>
#include <string>

class SystemMonitor {
public:
    SystemMonitor();

    // Triggers and retrieves system-wide CPU usage percentage
    double GetCPUUtilization();

    // Retrieves physical memory usage statistics
    bool GetMemoryStats(MemoryStats& stats);

    // Enumerates all active processes
    std::vector<ProcessInfo> GetProcesses();

    // Filters a list of processes by name match
    std::vector<ProcessInfo> FilterProcesses(const std::vector<ProcessInfo>& list, const std::string& searchStr);

    // Sorts a list of processes based on criteria ("PID", "NAME", "MEMORY")
    void SortProcesses(std::vector<ProcessInfo>& list, const std::string& sortBy, bool ascending = true);

    // Retrieves details of a specific process. Returns true if found.
    bool GetProcessDetails(unsigned long pid, ProcessInfo& outDetails);

    // Terminate a process by PID
    bool TerminateProcess(unsigned long pid, std::string& outErrorMsg);

private:
    CPUInfo cpuInfo;
    MemoryInfo memoryInfo;
    ProcessManager processManager;
};
