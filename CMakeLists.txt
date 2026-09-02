cmake_minimum_required(VERSION 3.12)
project(WinTaskMonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Unicode support for Windows APIs
add_compile_definitions(UNICODE _UNICODE)

# Compiler specific options for MSVC
if(MSVC)
    add_compile_options(/W4 /WX /EHsc)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# Core static library containing all monitoring logic
add_library(wintask_core STATIC
    src/ProcessManager.cpp
    src/CPUInfo.cpp
    src/MemoryInfo.cpp
    src/SystemMonitor.cpp
    src/Utils.cpp
    src/Logger.cpp
)

# Include directories
target_include_directories(wintask_core PUBLIC include)

# Link Windows system libraries
# psapi is needed for GetProcessMemoryInfo
target_link_libraries(wintask_core PUBLIC psapi)

# Main executable target
add_executable(WinTaskMonitor src/main.cpp)
target_link_libraries(WinTaskMonitor PRIVATE wintask_core)

# Test executable target
add_executable(WinTaskMonitorTests tests/basic_tests.cpp)
target_link_libraries(WinTaskMonitorTests PRIVATE wintask_core)
