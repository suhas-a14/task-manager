#include "Utils.h"
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace Utils {

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) {
        return "";
    }
    
    // Calculate the size needed for conversion
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    std::string strTo(sizeNeeded, 0);
    
    // Perform actual conversion
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0], sizeNeeded, nullptr, nullptr);
    return strTo;
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) {
        return L"";
    }
    
    // Calculate the size needed for conversion
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
    std::wstring wstrTo(sizeNeeded, 0);
    
    // Perform actual conversion
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0], sizeNeeded);
    return wstrTo;
}

std::string FormatBytes(unsigned long long bytes) {
    double kb = bytes / 1024.0;
    double mb = kb / 1024.0;
    double gb = mb / 1024.0;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    if (gb >= 1.0) {
        ss << gb << " GB";
    } else if (mb >= 1.0) {
        ss << mb << " MB";
    } else if (kb >= 1.0) {
        ss << kb << " KB";
    } else {
        ss << bytes << " B";
    }
    return ss.str();
}

std::string FormatPercentage(double percent) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << percent << "%";
    return ss.str();
}

bool IsNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    return std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c); });
}

}
