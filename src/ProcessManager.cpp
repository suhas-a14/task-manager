#include "ProcessManager.h"
#include "Utils.h"
#include "Logger.h"
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <algorithm>
#include <cctype>

// Helper function to fetch memory usage and handle count for a process.
// Demonstrates Windows API resource management and error handling.
static void PopulateDynamicDetails(ProcessInfo& info) {
    // Attempt to open the process with query rights
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info.pid);
    
    if (hProcess == nullptr) {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            info.status = "Access Denied";
        } else {
            info.status = "Unavailable";
        }
        info.memoryUsage = 0;
        info.handleCount = 0;
        return;
    }

    info.status = "Running";

    // Query private working set memory usage
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        info.memoryUsage = pmc.WorkingSetSize;
    } else {
        info.memoryUsage = 0;
    }

    // Query handle count
    DWORD handleCount = 0;
    if (GetProcessHandleCount(hProcess, &handleCount)) {
        info.handleCount = handleCount;
    } else {
        info.handleCount = 0;
    }

    // Crucial step: Release the kernel object handle to avoid system resource leaks
    CloseHandle(hProcess);
}

std::vector<ProcessInfo> ProcessManager::EnumerateProcesses() {
    std::vector<ProcessInfo> processes;

    // Create a snapshot of all processes in the system
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        Logger::Log(LogLevel::ERR, "Failed to create toolhelp snapshot. GetLastError: " + std::to_string(GetLastError()));
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process in the snapshot
    if (!Process32First(hSnapshot, &pe32)) {
        Logger::Log(LogLevel::ERR, "Failed to retrieve first process from snapshot. GetLastError: " + std::to_string(GetLastError()));
        CloseHandle(hSnapshot);
        return processes;
    }

    // Walk the snapshot of processes
    do {
        ProcessInfo info;
        info.pid = pe32.th32ProcessID;
        
        // Convert Wide Character string to standard multi-byte string
        info.name = Utils::WStringToString(pe32.szExeFile);
        
        // Skip System Idle Process (PID 0) from memory querying as it triggers access errors,
        // but include it in the output process list.
        if (info.pid != 0) {
            PopulateDynamicDetails(info);
        } else {
            info.name = "System Idle Process";
            info.status = "Running";
            info.memoryUsage = 0;
            info.handleCount = 0;
        }

        processes.push_back(info);
    } while (Process32Next(hSnapshot, &pe32));

    // Release snapshot handle
    CloseHandle(hSnapshot);

    return processes;
}

std::vector<ProcessInfo> ProcessManager::FilterProcessesByName(const std::vector<ProcessInfo>& processes, const std::string& searchStr) {
    std::vector<ProcessInfo> filtered;
    if (searchStr.empty()) {
        return processes;
    }

    std::string searchLower = searchStr;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    for (const auto& proc : processes) {
        std::string procLower = proc.name;
        std::transform(procLower.begin(), procLower.end(), procLower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        if (procLower.find(searchLower) != std::string::npos) {
            filtered.push_back(proc);
        }
    }

    return filtered;
}

void ProcessManager::SortByPID(std::vector<ProcessInfo>& processes, bool ascending) {
    std::sort(processes.begin(), processes.end(), [ascending](const ProcessInfo& a, const ProcessInfo& b) {
        return ascending ? (a.pid < b.pid) : (a.pid > b.pid);
    });
}

void ProcessManager::SortByName(std::vector<ProcessInfo>& processes, bool ascending) {
    std::sort(processes.begin(), processes.end(), [ascending](const ProcessInfo& a, const ProcessInfo& b) {
        return ascending ? (a.name < b.name) : (a.name > b.name);
    });
}

void ProcessManager::SortByMemory(std::vector<ProcessInfo>& processes, bool ascending) {
    std::sort(processes.begin(), processes.end(), [ascending](const ProcessInfo& a, const ProcessInfo& b) {
        return ascending ? (a.memoryUsage < b.memoryUsage) : (a.memoryUsage > b.memoryUsage);
    });
}

bool ProcessManager::GetProcessDetails(unsigned long pid, ProcessInfo& outDetails) {
    std::vector<ProcessInfo> currentProcs = EnumerateProcesses();
    for (const auto& proc : currentProcs) {
        if (proc.pid == pid) {
            // Re-fetch details to ensure real-time counts
            outDetails = proc;
            if (pid != 0) {
                PopulateDynamicDetails(outDetails);
            }
            return true;
        }
    }
    return false;
}

bool ProcessManager::TerminateProcessByPID(unsigned long pid, std::string& outErrorMsg) {
    // Attempt to open the process with termination rights
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    
    if (hProcess == nullptr) {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            outErrorMsg = "Access Denied (insufficient privileges). Run application as Administrator.";
            Logger::Log(LogLevel::WARNING, "Access Denied opening PID " + std::to_string(pid) + " for termination.");
        } else if (err == ERROR_INVALID_PARAMETER) {
            outErrorMsg = "Invalid Process ID. The process may have already exited.";
            Logger::Log(LogLevel::WARNING, "Attempted to terminate non-existent PID " + std::to_string(pid));
        } else {
            outErrorMsg = "Failed to open process. System Error: " + std::to_string(err);
            Logger::Log(LogLevel::ERR, "OpenProcess failed on PID " + std::to_string(pid) + " with error code " + std::to_string(err));
        }
        return false;
    }

    // Call TerminateProcess with exit code 1
    if (!TerminateProcess(hProcess, 1)) {
        DWORD err = GetLastError();
        outErrorMsg = "TerminateProcess failed. System Error: " + std::to_string(err);
        Logger::Log(LogLevel::ERR, "TerminateProcess failed on PID " + std::to_string(pid) + " with error code " + std::to_string(err));
        CloseHandle(hProcess);
        return false;
    }

    // Release process handle
    CloseHandle(hProcess);
    Logger::Log(LogLevel::INFO, "Successfully terminated process PID " + std::to_string(pid));
    return true;
}
