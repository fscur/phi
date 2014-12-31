#ifndef _PHI_PATH_H_
#define _PHI_PATH_H_

#include <string>
#include "io.h"

namespace phi
{
    class path
    {
    public:
        IO_API static std::string getFileNameWithoutExtension(std::string path);
    };
}

#endif