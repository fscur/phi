#ifndef _PHI_PATH_H_
#define _PHI_PATH_H_

#include <string>
#include <vector>
#include "io.h"
#include "fileInfo.h"
#include "directoryInfo.h"

namespace phi
{
    class path
    {
    public:
        IO_API static bool exists(std::string path);
        IO_API static std::string getDirectoryFullName(std::string path);
        IO_API static std::string getFileName(std::string path);
        IO_API static std::string getFileNameWithoutExtension(std::string path);
        IO_API static std::string getExtension(std::string path);
        IO_API static std::vector<fileInfo> getFiles(const std::string directory);
        IO_API static std::vector<directoryInfo> getDirectories(const std::string directory);
        IO_API static std::string combine(const std::string path0, const std::string path1);
    };
}

#endif