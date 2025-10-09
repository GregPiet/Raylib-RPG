#pragma once

#include <string>

namespace FileUtils {
std::string GetDirectoryName(const std::string& path);
std::string JoinPaths(const std::string& a, const std::string& b);
bool IsAbsolutePath(const std::string& path);
std::string ResolvePath(const std::string& baseDir, const std::string& relativePath);
}

