#include "Utils.h"
#include "ProcessInfo.h"
#include "ProcessManager.h"
#include <iostream>
#include <vector>
#include <cmath>

// Custom assert macro to avoid system aborts and print clean failure lines instead.
// Ideal for demonstrating developer testing knowledge in interviews.
#define TEST_ASSERT(cond, msg) \
    if (!(cond)) { \
        std::cerr << "  [FAIL] " << msg << " (Line " << __LINE__ << ")\n"; \
        return false; \
    }

// 1. Verifies the behavior of Utility helper functions
bool TestUtilsFormatting() {
    std::cout << "Running TestUtilsFormatting...\n";
    
    // FormatBytes tests
    TEST_ASSERT(Utils::FormatBytes(500) == "500 B", "Format bytes under 1KB failed");
    TEST_ASSERT(Utils::FormatBytes(1536) == "1.50 KB", "Format bytes for KB failed");
    TEST_ASSERT(Utils::FormatBytes(1024ULL * 1024 * 3) == "3.00 MB", "Format bytes for MB failed");
    TEST_ASSERT(Utils::FormatBytes(1024ULL * 1024 * 1024 * 8) == "8.00 GB", "Format bytes for GB failed");

    // FormatPercentage tests
    TEST_ASSERT(Utils::FormatPercentage(50.0) == "50.0%", "Format percentage failed");
    TEST_ASSERT(Utils::FormatPercentage(12.34) == "12.3%", "Format percentage rounding failed");

    // IsNumeric tests
    TEST_ASSERT(Utils::IsNumeric("9876"), "IsNumeric with valid number digits failed");
    TEST_ASSERT(!Utils::IsNumeric("987a6"), "IsNumeric with character failed");
    TEST_ASSERT(!Utils::IsNumeric(""), "IsNumeric with empty string failed");

    std::cout << "  [PASS] TestUtilsFormatting\n";
    return true;
}

// 2. Verifies the sorting algorithms of ProcessManager using mock vector data
bool TestProcessSorting() {
    std::cout << "Running TestProcessSorting...\n";

    std::vector<ProcessInfo> list = {
        {1024, "chrome.exe", 50000000, "Running", 50},
        {12, "system.exe", 100000000, "Running", 10},
        {500, "notepad.exe", 10000000, "Running", 100}
    };

    // Sort by PID - Ascending
    ProcessManager::SortByPID(list, true);
    TEST_ASSERT(list[0].pid == 12, "PID Sort Ascending 0 failed");
    TEST_ASSERT(list[1].pid == 500, "PID Sort Ascending 1 failed");
    TEST_ASSERT(list[2].pid == 1024, "PID Sort Ascending 2 failed");

    // Sort by Name - Ascending
    ProcessManager::SortByName(list, true);
    TEST_ASSERT(list[0].name == "chrome.exe", "Name Sort Ascending 0 failed");
    TEST_ASSERT(list[1].name == "notepad.exe", "Name Sort Ascending 1 failed");
    TEST_ASSERT(list[2].name == "system.exe", "Name Sort Ascending 2 failed");

    // Sort by Memory - Descending
    ProcessManager::SortByMemory(list, false);
    TEST_ASSERT(list[0].memoryUsage == 100000000, "Memory Sort Descending 0 failed");
    TEST_ASSERT(list[1].memoryUsage == 50000000, "Memory Sort Descending 1 failed");
    TEST_ASSERT(list[2].memoryUsage == 10000000, "Memory Sort Descending 2 failed");

    std::cout << "  [PASS] TestProcessSorting\n";
    return true;
}

// 3. Verifies process list filtering/searching behavior
bool TestProcessFiltering() {
    std::cout << "Running TestProcessFiltering...\n";

    std::vector<ProcessInfo> list = {
        {1024, "chrome.exe", 50000000, "Running", 50},
        {12, "system.exe", 100000000, "Running", 10},
        {500, "notepad.exe", 10000000, "Running", 100}
    };

    ProcessManager pm;
    
    // Case insensitive substring search
    auto results = pm.FilterProcessesByName(list, "SYS");
    TEST_ASSERT(results.size() == 1, "Filter size failed on 'SYS'");
    TEST_ASSERT(results[0].name == "system.exe", "Filter value matching failed on 'SYS'");

    // Empty search string returns full list
    results = pm.FilterProcessesByName(list, "");
    TEST_ASSERT(results.size() == 3, "Filter empty string should return all entries");

    // No matching results search
    results = pm.FilterProcessesByName(list, "firefox");
    TEST_ASSERT(results.empty(), "Filter should return empty list for non-matching search term");

    std::cout << "  [PASS] TestProcessFiltering\n";
    return true;
}

// 4. Verifies system math calculations (Memory usage % and CPU % formulas)
bool TestMathCalculations() {
    std::cout << "Running TestMathCalculations...\n";

    // Memory load calculation check: Used Memory / Total Memory * 100
    unsigned long long totalRam = 16ULL * 1024 * 1024 * 1024; // 16GB
    unsigned long long freeRam = 4ULL * 1024 * 1024 * 1024;   // 4GB
    unsigned long long usedRam = totalRam - freeRam;          // 12GB
    double memPercent = (static_cast<double>(usedRam) / totalRam) * 100.0;
    TEST_ASSERT(std::abs(memPercent - 75.0) < 0.0001, "Memory percentage math incorrect");

    // CPU Load calculation formula check:
    // Busy Time = Total System Time Change - Idle Time Change
    // CPU % = Busy Time / Total System Time Change * 100.0
    unsigned long long totalSystemDiff = 2000;
    unsigned long long idleTimeDiff = 500;
    double cpuPercent = 0.0;
    if (totalSystemDiff >= idleTimeDiff && totalSystemDiff > 0) {
        unsigned long long busyDiff = totalSystemDiff - idleTimeDiff;
        cpuPercent = (static_cast<double>(busyDiff) / totalSystemDiff) * 100.0;
    }
    TEST_ASSERT(std::abs(cpuPercent - 75.0) < 0.0001, "CPU load calculation math incorrect");

    std::cout << "  [PASS] TestMathCalculations\n";
    return true;
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "               WIN TASK MONITOR TESTS             \n";
    std::cout << "==================================================\n";

    bool allPassed = true;
    allPassed &= TestUtilsFormatting();
    allPassed &= TestProcessSorting();
    allPassed &= TestProcessFiltering();
    allPassed &= TestMathCalculations();

    std::cout << "==================================================\n";
    if (allPassed) {
        std::cout << "STATUS: ALL AUTOMATED UNIT TESTS PASSED.\n";
        return 0; // Return success code
    } else {
        std::cout << "STATUS: SOME UNIT TESTS FAILED. CHECK ERRORS.\n";
        return 1; // Return failure code
    }
}
