# WinTask Monitor

A lightweight, high-performance Windows system resource monitoring and process management application written in modern C++17. Designed as an educational intermediate-level project to demonstrate C++ core mechanics, Windows API integration, and Operating System principles.

---

## 1. Project Overview & Problem Statement

Developing software on Windows requires a solid understanding of OS-level concepts (processes, memory allocation, handles, and CPU scheduling). While Microsoft provides the default Task Manager, understanding *how* it interacts with the Windows Kernel is crucial for system engineers. 

**WinTask Monitor** solves this by providing a clean, terminal-based resource monitor that queries live system metrics directly from the Windows kernel. It displays:
1. Overall CPU utilization (computed from actual system times).
2. Physical memory usage (computed from kernel memory buffers).
3. Active processes, including PIDs, names, private working set sizes, and status.
4. An interactive TUI for searching, sorting, inspecting details, and terminating target processes.

---

## 2. Core Features

- **Resource Dashboard:** Displays real-time overall CPU utilization and physical RAM metrics.
- **Process Enumeration:** Lists all running process names, PIDs, and memory allocations.
- **Sorting & Search:** Sort processes by Name, PID, or Memory usage; search for active processes using case-insensitive substring filters.
- **Process Details:** Inspect handle counts and precise memory sizes for target PIDs.
- **Safe Process Termination:** Terminate user processes with double confirmation prompts, administrative privilege warnings, and error reports.
- **Auto-Refresh Loop:** Features a non-blocking input loop that refreshes system states every 1.5 seconds.

---

## 3. Technologies Used

- **Operating System:** Windows 10/11
- **Language Standard:** C++17
- **Compiler:** Microsoft Visual C++ Compiler (MSVC)
- **Build System:** CMake 3.12+
- **API Surface:** Win32 SDK (Process snapshots, PSAPI memory statistics, Kernel Handle interfaces)
- **Standard Library:** C++ STL (`std::vector`, `std::string`, `std::sort`, `std::optional`, thread synchronization)

---

## 4. Architecture & Directory Structure

WinTask Monitor separates presentation logic from system-level API integration:

```text
WinTaskMonitor/ (Workspace Root)
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # Primary documentation
│
├── include/                    # Header files
│   ├── ProcessInfo.h           # Process data model struct
│   ├── ProcessManager.h        # Process querying, sorting, and termination
│   ├── CPUInfo.h               # CPU system times delta calculation
│   ├── MemoryInfo.h            # RAM usage querying
│   ├── SystemMonitor.h         # Aggregate facade coordinator
│   ├── Utils.h                 # Unicode converters and format helpers
│   └── Logger.h                # Simple logging definitions
│
├── src/                        # Implementations
│   ├── main.cpp                # Presentation TUI loop
│   ├── ProcessManager.cpp
│   ├── CPUInfo.cpp
│   ├── MemoryInfo.cpp
│   ├── SystemMonitor.cpp
│   ├── Utils.cpp
│   └── Logger.cpp
│
├── tests/
│   └── basic_tests.cpp         # Automated business logic tests
│
└── docs/
    ├── architecture.md         # Component diagrams and data flow
    └── interview_questions.md  # 20+ interview Q&As
```

---

## 5. System Concepts & Mathematical Calculations

### A. CPU Utilization
Overall CPU load is calculated from cumulative times retrieved via `GetSystemTimes()`.
```text
SystemTimeDiff = (KernelTime2 - KernelTime1) + (UserTime2 - UserTime1)
IdleTimeDiff   = IdleTime2 - IdleTime1
BusyTimeDiff   = SystemTimeDiff - IdleTimeDiff
CPU Usage %    = (BusyTimeDiff / SystemTimeDiff) * 100.0
```
*Note: Because KernelTime includes IdleTime in Windows, we subtract IdleTimeDiff from SystemTimeDiff to isolate actual active CPU usage.*

### B. Memory Utilization
Physical memory load is queried using `GlobalMemoryStatusEx()` and calculated manually:
```text
Used Memory    = Total Physical Memory - Available Physical Memory
Memory Usage % = (Used Memory / Total Physical Memory) * 100.0
```

### C. Process Enumeration
We obtain a list of PIDs and names using `CreateToolhelp32Snapshot()`, `Process32First()`, and `Process32Next()`. For each valid process, we attempt to open a handle with `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ` to query its Working Set size (memory) and open handles count.

### D. Process Termination
We request termination of a process using `OpenProcess(PROCESS_TERMINATE, FALSE, pid)` and execute `TerminateProcess(hProcess, 1)`. If the target process is protected, we gracefully report the system error `ERROR_ACCESS_DENIED`.

---

## 6. How to Build and Run

Make sure you have **Visual Studio (MSVC)** and **CMake** installed.

### Build Instructions
Open Developer Command Prompt/PowerShell and execute the following commands in the workspace root:

```bash
# Generate the build files
cmake -S . -B build

# Compile the project targets
cmake --build build --config Release
```

This will generate two executable targets:
1. `build/Release/WinTaskMonitor.exe` (Main TUI application)
2. `build/Release/WinTaskMonitorTests.exe` (Automated unit tests)

### Run Main Application
```bash
./build/Release/WinTaskMonitor.exe
```

### Run Unit Tests
```bash
./build/Release/WinTaskMonitorTests.exe
```

---

## 7. Example Console Dashboard

```text
======================================================================
                          WIN TASK MONITOR                            
======================================================================
CPU Usage:       12.4%
Memory Usage:    56.8% (9.09 GB / 16.00 GB)
Available RAM:   6.91 GB
----------------------------------------------------------------------
Sort Column:     MEMORY (Descending)
Search Filter:   [None]
----------------------------------------------------------------------
PID       PROCESS NAME                    MEMORY        STATUS        
----------------------------------------------------------------------
14240     chrome.exe                      684.23 MB     Running       
8921      slack.exe                       342.10 MB     Running       
3240      explorer.exe                    210.50 MB     Running       
1504      notepad.exe                     23.42 MB      Running       
0         System Idle Process             N/A           Running       
----------------------------------------------------------------------
[Showing top 5 of 180 processes. Choose command 2 to filter.]

Commands:
  [1] Force Refresh      [4] Sort by Name      [7] End Process
  [2] Search/Filter      [5] Sort by Memory    [8] Exit
  [3] Sort by PID        [6] Process Details

Enter command (1-8) [Auto-refreshing every 1.5s...]: 
```

---

## 8. Limitations & Future Scope

1. **Per-Process CPU Time:** The current version calculates system-wide CPU utilization. In the future, we could query `GetProcessTimes()` over an interval to compute resource consumption of specific processes.
2. **Handle Count Access:** Handle counts are only queryable for processes owned by the user. Administrative processes will display `"Access Denied"` memory states unless the monitor itself is run with elevated administrator rights.
3. **Cross-Platform:** The core library is strictly tied to Windows kernel calls. An abstraction layer can be implemented in Version 2 to support Linux (parsing `/proc`).
