#pragma once
#include <precompiled.h>
#include "io.h"
#include "fileInfo.h"
#include "directoryInfo.h"

namespace phi
{
    class path
    {
    public:
        IO_API static bool exists(const string& path);
        IO_API static string getDirectoryFullName(string path);
        IO_API static string getFileName(string path);
        IO_API static string getFileNameWithoutExtension(string path);
        IO_API static string getExtension(string path);
        IO_API static vector<fileInfo> getFiles(const string& directory, vector<string> filters = vector<string>());
        IO_API static vector<directoryInfo> getDirectories(const string& directory);
        IO_API static string combine(const string& path0, const string& path1, const string& extension = string());
        IO_API static string combine(std::initializer_list<string> args = std::initializer_list<string>());
    };
}