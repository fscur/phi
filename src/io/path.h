#pragma once

#include "io.h"
#include "fileInfo.h"
#include "directoryInfo.h"

#include <vector>
#include <stdarg.h>

namespace phi
{
    class path
    {
    public:
        IO_API static bool exists(const std::string& path);
        IO_API static std::string getDirectoryFullName(std::string path);
        IO_API static std::string getFileName(std::string path);
        IO_API static std::string getFileNameWithoutExtension(std::string path);
        IO_API static std::string getExtension(std::string path);
        IO_API static std::vector<fileInfo> getFiles(const std::string& directory, std::vector<std::string> filters = std::vector<std::string>());
        IO_API static std::vector<directoryInfo> getDirectories(const std::string& directory);
        IO_API static std::string combine(const std::string& path0, const std::string& path1, const std::string& extension = std::string());
        IO_API static std::string combine(std::initializer_list<std::string> args = std::initializer_list<std::string>());
    };
}