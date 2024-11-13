#pragma once

#include <string>
#include <algorithm>

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    size_t length = end - start + 1;
    
    return str.substr(start, length);
}

