#pragma once
#include <string>

namespace Utils {
    // Formats a byte size into a human-readable string (e.g. "1.23 GB", "450.0 MB", "12 KB", "512 B")
    std::string FormatBytes(unsigned long long bytes);
    
    // Formats a percentage value to a fixed-decimal string (e.g. "45.2%")
    std::string FormatPercentage(double percent);
    
    // Converts a Windows wide string (std::wstring) to a standard UTF-8 string (std::string)
    std::string WStringToString(const std::wstring& wstr);
    
    // Converts a standard UTF-8 string (std::string) to a Windows wide string (std::wstring)
    std::wstring StringToWString(const std::string& str);
    
    // Helper to check if a string consists entirely of digits
    bool IsNumeric(const std::string& str);
}
