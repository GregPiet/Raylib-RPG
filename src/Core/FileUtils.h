#pragma once
#include <string>
#include <raylib.h>
#include <cctype>

namespace FileUtils {

    inline std::string GetDirectoryName(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return "";
        return path.substr(0, pos);
    }

    inline std::string JoinPaths(const std::string& a, const std::string& b) {
        if (a.empty()) return b;
        if (b.empty()) return a;

        std::string pathA = a, pathB = b;
        if (!pathA.empty() && (pathA.back() == '/' || pathA.back() == '\\'))
            pathA.pop_back();
        if (!pathB.empty() && (pathB.front() == '/' || pathB.front() == '\\'))
            pathB.erase(0, 1);

        return pathA + "/" + pathB;
    }

    inline bool IsAbsolutePath(const std::string& path) {
        if (path.empty()) return false;
        #ifdef _WIN32
        if (path.size() > 1 && std::isalpha((unsigned char)path[0]) && path[1] == ':')
            return true;
        #endif
        return path[0] == '/' || path[0] == '\\';
    }

    inline std::string ResolvePath(const std::string& baseDir, const std::string& relativePath) {
        if (relativePath.empty() || IsAbsolutePath(relativePath))
            return relativePath;

        std::string candidate = baseDir.empty() ? relativePath : JoinPaths(baseDir, relativePath);
        if (FileExists(candidate.c_str()))
            return candidate;

        if (FileExists(relativePath.c_str()))
            return relativePath;

        return candidate;
    }
}
