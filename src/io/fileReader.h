#pragma once
#include <phi.h>
#include "ioApi.h"

namespace phi
{
    class IO_API fileReader
    {
    public:
        static string readFile(const string& filePath);
    };
}