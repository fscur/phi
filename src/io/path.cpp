#include <precompiled.h>
#include "path.h"

#include <core\exception.h>

namespace phi
{
    bool path::exists(const string& path)
    {
        struct stat s;
        int err = stat(path.c_str(), &s);
        if (err == -1) 
        {
            if (ENOENT == errno)
                return false;

            throw exception("[path::exists] unknown error.");
        }

        return true;
    }

    string path::getDirectoryFullName(string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        auto slashIndex = path.find_last_of('\\');

        return path.substr(0, path.size() - (path.size() - slashIndex));
    }

    string path::getFileName(string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        auto slashIndex = path.find_last_of('\\');

        return path.substr(slashIndex + 1, path.size() - slashIndex - 1);
    }

    string path::getFileNameWithoutExtension(string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        auto slashIndex = path.find_last_of('\\');
        auto dotIndex = path.find_last_of('.');

        if (dotIndex < slashIndex)
            return path.substr(0, dotIndex);

        return path.substr(slashIndex + 1, dotIndex - slashIndex - 1);
    }

    string path::getExtension(string path)
    {
        int dotIndex = static_cast<int>(path.find_last_of('.'));

        if (dotIndex < 0)
            return "";

        return path.substr(dotIndex, path.length() - dotIndex);
    }

    vector<fileInfo> path::getFiles(
        const string& directory, 
        vector<string> filters)
    {
#ifdef _WIN32
        vector<fileInfo> out;
        HANDLE dir;
        WIN32_FIND_DATA file_data;

        if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
            return out; /* No files found */

        bool filterByExtension = filters.size() > 0;

        do 
        {
            const string file_name = file_data.cFileName;
            const string full_file_name = directory + "\\" + file_name;
            const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            auto fileExtension = getExtension(file_name);

            if (filterByExtension && !phi::contains(filters, fileExtension))
                continue;

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
        return vector<fileInfo>();
#endif
    }

    vector<directoryInfo> path::getDirectories(const string& directory)
    {
#ifdef _WIN32
        vector<directoryInfo> out;
        HANDLE dir;
        WIN32_FIND_DATA file_data;

        if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
            return out; /* No files found */

        do 
        {

            const string file_name = file_data.cFileName;
            const string full_file_name = directory + "\\" + file_name;
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
        return vector<directoryInfo>();
#endif
    }

    string path::combine(const string& path0, const string& path1, const string& extension)
    {   
        auto combined = path0 + "\\" + path1 + extension;
        std::replace(combined.begin(), combined.end(), '/', '\\');
        return combined;
    }

    string path::combine(std::initializer_list<string> args)
    {
        auto combined = string();

        for(auto arg : args)
        {
            combined += "\\" + arg;
        }

        std::replace(combined.begin(), combined.end(), '/', '\\');

        return combined;
    }
}