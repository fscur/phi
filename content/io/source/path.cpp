#include <algorithm>
#include "path.h"
#include <vector>
#include <string>
#include <windows.h>

namespace phi
{
    std::string path::getFileName(std::string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        int slashIndex = path.find_last_of('\\');

        return path.substr(slashIndex + 1, path.size() - slashIndex - 1);
    }

    std::string path::getFileNameWithoutExtension(std::string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        int slashIndex = path.find_last_of('\\');
        int dotIndex = path.find_last_of('.');

        if (dotIndex < slashIndex)
            return "";

        return path.substr(slashIndex + 1, dotIndex - slashIndex - 1);
    }

    std::vector<fileInfo> path::getFiles(const std::string directory)
    {
    #ifdef WIN32
        std::vector<fileInfo> out;
        HANDLE dir;
        WIN32_FIND_DATA file_data;

        if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
            return out; /* No files found */

        do 
        {
            const std::string file_name = file_data.cFileName;
            const std::string full_file_name = directory + "/" + file_name;
            const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            fileInfo info;
            info.name = file_name;
            info.path = full_file_name;

            if (file_name[0] == '.')
                continue;

            if (is_directory)
                continue;

            out.push_back(info);
        } while (FindNextFile(dir, &file_data));

        FindClose(dir);

        return out;
    #else
        DIR *dir;
        class dirent *ent;
        class stat st;

        dir = opendir(directory);
        while ((ent = readdir(dir)) != NULL) {
            const std::string file_name = ent->d_name;
            const std::string full_file_name = directory + "/" + file_name;

            if (file_name[0] == '.')
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            out.push_back(full_file_name);
        }
        closedir(dir);
    #endif
    }

    std::vector<directoryInfo> path::getDirectories(const std::string directory)
    {
    #ifdef WIN32
        std::vector<directoryInfo> out;
        HANDLE dir;
        WIN32_FIND_DATA file_data;

        if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
            return out; /* No files found */

        do 
        {
            
            const std::string file_name = file_data.cFileName;
            const std::string full_file_name = directory + "/" + file_name;
            const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            directoryInfo info;
            info.name = file_name;
            info.path = full_file_name;

            if (file_name[0] == '.')
                continue;

            if (!is_directory)
                continue;

            out.push_back(info);

        } while (FindNextFile(dir, &file_data));

        FindClose(dir);

        return out;
    #else
        DIR *dir;
        class dirent *ent;
        class stat st;

        dir = opendir(directory);
        while ((ent = readdir(dir)) != NULL) {
            const std::string file_name = ent->d_name;
            const std::string full_file_name = directory + "/" + file_name;

            if (file_name[0] == '.')
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            out.push_back(full_file_name);
        }
        closedir(dir);
    #endif
    }
}