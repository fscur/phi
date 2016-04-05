#pragma once
#include <phi.h>
#include "ioApi.h"
#include "fileInfo.h"
#include "directoryInfo.h"

namespace phi
{
    class IO_API path
    {
    public:
        static bool exists(const string& path);
        static string getDirectoryFullName(string path);
        static string getFileName(string path);
        static string getFileNameWithoutExtension(string path);
        static string getExtension(string path);
        static vector<fileInfo> getFiles(const string& directory, vector<string> filters = vector<string>());
        static vector<directoryInfo> getDirectories(const string& directory);
        static string combine(const string& path0, const string& path1, const string& extension = string());
        static string combine(std::initializer_list<string> args = std::initializer_list<string>());
    };
}