#include <algorithm>
#include "path.h"

namespace phi
{
    std::string path::getFileNameWithoutExtension(std::string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        int slashIndex = path.find_last_of('\\');
        int dotIndex = path.find_last_of('.');

        if (dotIndex < slashIndex)
            return "";

        return path.substr(slashIndex + 1, dotIndex - slashIndex - 1);
    }
}