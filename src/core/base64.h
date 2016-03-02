#pragma once

#include "globals.h"

namespace phi
{
    class base64
    {
    public:
        CORE_API static std::string encode(byte const* buf, unsigned int bufLen);
        CORE_API static std::vector<byte> decode(std::string const&);
    };
}