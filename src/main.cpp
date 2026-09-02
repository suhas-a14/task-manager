#include "SystemMonitor.h"
#include "Utils.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <algorithm>

void ClearScreen() {
    // Standard Windows terminal clear screen command.
    // Extremely reliable and native for Windows Command Prompt/PowerShell environments.
    system("cls");
}

int main() {
    // Configure log file name
    Logger::SetLogFile("wintask_monitor.log");
    Logger::Log(LogLevel::INFO, "WinTask Monitor started successfully.");

    SystemMonitor monitor;
    
    // Sort configuration state
    std::string sortBy = "MEMORY";
    bool sortAscending = false; // Descending memory sorting shows resource heavy items first
    
    // Filtering state
    std::string searchFilter = "";

    bool running = true;

    while (running) {
        ClearScreen();

        // 1. Query resource monitors
        double cpuUsage = monitor.GetCPUUtilization();
        MemoryStats memStats;
        bool memSuccess = monitor.GetMemoryStats(memStats);

        // 2. Query process manager
        std::vector<ProcessInfo> processes = monitor.GetProcesses();
        
        // Filter processes
        std::vector<ProcessInfo> displayedProcesses = processes;
        if (!searchFilter.empty()) {
            displayedProcesses = monitor.FilterProcesses(processes, searchFilter);
        }

        // Sort processes
        monitor.SortProcesses(displayedProcesses, sortBy, sortAscending);

        // 3. Render Dashboard TUI
        std::cout << "======================================================================\n";
        std::cout << "                          WIN TASK MONITOR                            \n";
        std::cout << "======================================================================\n";
        std::cout << "CPU Usage:       " << Utils::FormatPercentage(cpuUsage) << "\n";
        
        if (memSuccess) {
            std::cout << "Memory Usage:    " << Utils::FormatPercentage(memStats.percentageUsed) 
                      << " (" << Utils::FormatBytes(memStats.usedPhysical) << " / " 
                      << Utils::FormatBytes(memStats.totalPhysical) << ")\n";
            std::cout << "Available RAM:   " << Utils::FormatBytes(memStats.availablePhysical) << "\n";
        } else {
            std::cout << "Memory Usage:    [Error querying memory statistics]\n";
        }
        
        std::cout << "----------------------------------------------------------------------\n";
        std::cout << "Sort Column:     " << sortBy << " (" << (sortAscending ? "Ascending" : "Descending") << ")\n";
        std::cout << "Search Filter:   " << (searchFilter.empty() ? "[None]" : "\"" + searchFilter + "\"") << "\n";
        std::cout << "----------------------------------------------------------------------\n";
        
        // Print Process table headers
        std::cout << std::left 
                  << std::setw(10) << "PID" 
                  << std::setw(32) << "PROCESS NAME" 
                  << std::setw(14) << "MEMORY" 
                  << std::setw(14) << "STATUS" << "\n";
        std::cout << "----------------------------------------------------------------------\n";

        // Page the process display output to prevent terminal overflow.
        // If a search filter is set, show all matches. If no filter, show the top 20.
        size_t maxToDisplay = 20;
        size_t totalMatching = displayedProcesses.size();
        size_t printCount = (searchFilter.empty()) ? std::min(maxToDisplay, totalMatching) : totalMatching;

        if (totalMatching == 0) {
            std::cout << "               [No matching processes found]                  \n";
        } else {
            for (size_t i = 0; i < printCount; ++i) {
                const auto& proc = displayedProcesses[i];
                
                std::string memStr = (proc.pid == 0) ? "N/A" : Utils::FormatBytes(proc.memoryUsage);
                
                // Truncate process name if too long for layout
                std::string displayName = proc.name;
                if (displayName.length() > 29) {
                    displayName = displayName.substr(0, 26) + "...";
                }

                std::cout << std::left 
                          << std::setw(10) << proc.pid 
                          << std::setw(32) << displayName
                          << std::setw(14) << memStr
                          << std::setw(14) << proc.status << "\n";
            }
        }

        std::cout << "----------------------------------------------------------------------\n";
        if (searchFilter.empty()) {
            std::cout << "[Showing top " << printCount << " of " << totalMatching << " processes. Choose command 2 to filter.]\n";
        } else {
            std::cout << "[Found " << totalMatching << " matching processes]\n";
        }
        std::cout << "\n";
        std::cout << "Commands:\n";
        std::cout << "  [1] Force Refresh      [4] Sort by Name      [7] End Process\n";
        std::cout << "  [2] Search/Filter      [5] Sort by Memory    [8] Exit\n";
        std::cout << "  [3] Sort by PID        [6] Process Details\n";
        std::cout << "\n";
        std::cout << "Enter command (1-8) [Auto-refreshing every 1.5s...]: ";
        std::cout.flush();

        // 4. Polling loop for non-blocking console input
        // Checks _kbhit() every 100ms up to 15 times (totaling a 1.5-second refresh interval)
        bool keyPressed = false;
        char choice = 0;
        for (int i = 0; i < 15; ++i) {
            if (_kbhit()) {
                choice = static_cast<char>(_getch());
                keyPressed = true;
                break;
            }
            Sleep(100);
        }

        if (keyPressed) {
            if (choice == '1') {
                // Just trigger immediate refresh
                continue;
            }
            else if (choice == '2') {
                std::cout << "\nEnter Search Term (leave empty to clear filter): ";
                std::string filter;
                std::getline(std::cin, filter);
                searchFilter = filter;
            }
            else if (choice == '3') {
                if (sortBy == "PID") {
                    sortAscending = !sortAscending;
                } else {
                    sortBy = "PID";
                    sortAscending = true;
                }
            }
            else if (choice == '4') {
                if (sortBy == "NAME") {
                    sortAscending = !sortAscending;
                } else {
                    sortBy = "NAME";
                    sortAscending = true;
                }
            }
            else if (choice == '5') {
                if (sortBy == "MEMORY") {
                    sortAscending = !sortAscending;
                } else {
                    sortBy = "MEMORY";
                    sortAscending = false; // Descending by default for memory
                }
            }
            else if (choice == '6') {
                std::cout << "\nEnter PID: ";
                std::string pidStr;
                std::getline(std::cin, pidStr);
                if (!Utils::IsNumeric(pidStr)) {
                    std::cout << "Error: PID must be a valid numeric integer.\n";
                    std::cout << "Press any key to continue...";
                    _getch();
                    continue;
                }
                
                unsigned long pid = std::stoul(pidStr);
                ProcessInfo det;
                if (monitor.GetProcessDetails(pid, det)) {
                    std::cout << "\n==================================================\n";
                    std::cout << "                PROCESS DETAILS                  \n";
                    std::cout << "==================================================\n";
                    std::cout << "PID:            " << det.pid << "\n";
                    std::cout << "Name:           " << det.name << "\n";
                    std::cout << "Memory Usage:   " << Utils::FormatBytes(det.memoryUsage) << " (" << det.memoryUsage << " bytes)\n";
                    std::cout << "Handle Count:   " << det.handleCount << "\n";
                    std::cout << "Status:         " << det.status << "\n";
                    std::cout << "==================================================\n";
                } else {
                    std::cout << "\nError: Process PID " << pid << " was not found or has terminated.\n";
                }
                std::cout << "Press any key to return...";
                _getch();
            }
            else if (choice == '7') {
                std::cout << "\nEnter Process PID to terminate: ";
                std::string pidStr;
                std::getline(std::cin, pidStr);
                if (!Utils::IsNumeric(pidStr)) {
                    std::cout << "Error: PID must be a valid numeric integer.\n";
                    std::cout << "Press any key to continue...";
                    _getch();
                    continue;
                }
                
                unsigned long pid = std::stoul(pidStr);
                ProcessInfo det;
                if (!monitor.GetProcessDetails(pid, det)) {
                    std::cout << "\nError: Process PID " << pid << " was not found or has terminated.\n";
                    std::cout << "Press any key to continue...";
                    _getch();
                    continue;
                }
                
                std::cout << "\n";
                std::cout << "==================================================\n";
                std::cout << "WARNING: TERMINATE PROCESS SELECTION\n";
                std::cout << "==================================================\n";
                std::cout << "  PID:        " << det.pid << "\n";
                std::cout << "  Name:       " << det.name << "\n";
                std::cout << "  Memory:     " << Utils::FormatBytes(det.memoryUsage) << "\n";
                std::cout << "==================================================\n";
                std::cout << "Are you sure you want to terminate this process? (Y/N): ";
                
                char confirm = static_cast<char>(_getch());
                std::cout << "\n";
                if (confirm == 'Y' || confirm == 'y') {
                    std::string errorMsg;
                    if (monitor.TerminateProcess(pid, errorMsg)) {
                        std::cout << "\nSUCCESS: Process " << pid << " (" << det.name << ") terminated successfully.\n";
                    } else {
                        std::cout << "\nFAILURE: " << errorMsg << "\n";
                    }
                } else {
                    std::cout << "\nOperation aborted. Process " << pid << " was not harmed.\n";
                }
                std::cout << "Press any key to continue...";
                _getch();
            }
            else if (choice == '8' || choice == 'q' || choice == 'Q') {
                running = false;
            }
        }
    }

    std::cout << "\nExiting WinTask Monitor. Logging cleanup.\n";
    Logger::Log(LogLevel::INFO, "WinTask Monitor stopped cleanly by user.");
    return 0;
}
