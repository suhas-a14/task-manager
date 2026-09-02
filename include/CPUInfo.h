#pragma once

class CPUInfo {
public:
    CPUInfo();

    // Calculates and returns system-wide CPU utilization (%) since the last call.
    double CalculateCPUUtilization();

private:
    unsigned long long prevIdleTime = 0;
    unsigned long long prevKernelTime = 0;
    unsigned long long prevUserTime = 0;

    // Helper to call Windows system time APIs and return values as 64-bit integers
    bool GetSystemTimesRaw(unsigned long long& idle, unsigned long long& kernel, unsigned long long& user);
};
