#pragma once

struct MemoryStats {
    unsigned long long totalPhysical = 0;      // Total RAM in bytes
    unsigned long long availablePhysical = 0;  // Free RAM in bytes
    unsigned long long usedPhysical = 0;       // Used RAM in bytes
    double percentageUsed = 0.0;               // Computed memory usage percentage
};

class MemoryInfo {
public:
    // Retrieves current physical memory statistics from Windows. Returns true if successful.
    bool RetrieveMemoryStats(MemoryStats& stats);
};
