#include "CPUInfo.h"
#include "Logger.h"
#include <windows.h>

static unsigned long long FileTimeToULL(const FILETIME& ft) {
    return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

CPUInfo::CPUInfo() {
    // Perform an initial measurement during construction to establish a baseline
    GetSystemTimesRaw(prevIdleTime, prevKernelTime, prevUserTime);
}

bool CPUInfo::GetSystemTimesRaw(unsigned long long& idle, unsigned long long& kernel, unsigned long long& user) {
    FILETIME ftIdle, ftKernel, ftUser;
    if (!GetSystemTimes(&ftIdle, &ftKernel, &ftUser)) {
        Logger::Log(LogLevel::ERR, "Failed to call GetSystemTimes API. GetLastError: " + std::to_string(GetLastError()));
        return false;
    }
    idle = FileTimeToULL(ftIdle);
    kernel = FileTimeToULL(ftKernel);
    user = FileTimeToULL(ftUser);
    return true;
}

double CPUInfo::CalculateCPUUtilization() {
    unsigned long long currentIdle = 0;
    unsigned long long currentKernel = 0;
    unsigned long long currentUser = 0;

    if (!GetSystemTimesRaw(currentIdle, currentKernel, currentUser)) {
        return 0.0;
    }

    // Calculate changes since the last measurement
    unsigned long long idleDiff = currentIdle - prevIdleTime;
    unsigned long long kernelDiff = currentKernel - prevKernelTime;
    unsigned long long userDiff = currentUser - prevUserTime;

    // Total system time difference is kernel time change + user time change
    unsigned long long totalSystemDiff = kernelDiff + userDiff;

    double cpuPercentage = 0.0;
    if (totalSystemDiff > 0) {
        // Windows Kernel time includes Idle time.
        // Therefore, Busy time is Total Diff - Idle Diff.
        if (totalSystemDiff >= idleDiff) {
            unsigned long long busyDiff = totalSystemDiff - idleDiff;
            cpuPercentage = (static_cast<double>(busyDiff) / totalSystemDiff) * 100.0;
        } else {
            // Guard against edge cases where timer resolution reports more idle time than total time
            cpuPercentage = 0.0;
        }
    }

    // Cache current times for the next interval measurement
    prevIdleTime = currentIdle;
    prevKernelTime = currentKernel;
    prevUserTime = currentUser;

    return cpuPercentage;
}
