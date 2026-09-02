#include "SystemMonitor.h"

SystemMonitor::SystemMonitor() {}

double SystemMonitor::GetCPUUtilization() {
    return cpuInfo.CalculateCPUUtilization();
}

bool SystemMonitor::GetMemoryStats(MemoryStats& stats) {
    return memoryInfo.RetrieveMemoryStats(stats);
}

std::vector<ProcessInfo> SystemMonitor::GetProcesses() {
    return processManager.EnumerateProcesses();
}

std::vector<ProcessInfo> SystemMonitor::FilterProcesses(const std::vector<ProcessInfo>& list, const std::string& searchStr) {
    return processManager.FilterProcessesByName(list, searchStr);
}

void SystemMonitor::SortProcesses(std::vector<ProcessInfo>& list, const std::string& sortBy, bool ascending) {
    if (sortBy == "PID") {
        ProcessManager::SortByPID(list, ascending);
    } else if (sortBy == "NAME") {
        ProcessManager::SortByName(list, ascending);
    } else if (sortBy == "MEMORY") {
        ProcessManager::SortByMemory(list, ascending);
    }
}

bool SystemMonitor::GetProcessDetails(unsigned long pid, ProcessInfo& outDetails) {
    return processManager.GetProcessDetails(pid, outDetails);
}

bool SystemMonitor::TerminateProcess(unsigned long pid, std::string& outErrorMsg) {
    return processManager.TerminateProcessByPID(pid, outErrorMsg);
}
