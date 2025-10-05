#pragma once
#include <string>
#include <iostream>

inline void Log(const std::string& msg) {
    std::cout << "[INFO] " << msg << "\n";
}
