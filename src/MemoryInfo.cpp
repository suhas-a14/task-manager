#include "MemoryInfo.h"
#include "Logger.h"
#include <windows.h>

bool MemoryInfo::RetrieveMemoryStats(MemoryStats& stats) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    if (!GlobalMemoryStatusEx(&memInfo)) {
        Logger::Log(LogLevel::ERR, "Failed to call GlobalMemoryStatusEx API. GetLastError: " + std::to_string(GetLastError()));
        return false;
    }
    
    stats.totalPhysical = memInfo.ullTotalPhys;
    stats.availablePhysical = memInfo.ullAvailPhys;
    
    // Calculate used memory and percentage
    stats.usedPhysical = stats.totalPhysical - stats.availablePhysical;
    if (stats.totalPhysical > 0) {
        stats.percentageUsed = (static_cast<double>(stats.usedPhysical) / stats.totalPhysical) * 100.0;
    } else {
        stats.percentageUsed = 0.0;
    }
    
    return true;
}
