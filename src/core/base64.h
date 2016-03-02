#pragma once
#include <precompiled.h>
#include "core.h"

namespace phi
{
    class base64
    {
    public:
        CORE_API static string encode(byte const* buf, uint bufLen);
        CORE_API static vector<byte> decode(string const&);
    };
}