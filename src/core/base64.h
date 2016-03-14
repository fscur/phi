#pragma once
#include <phi.h>
#include "core.h"

namespace phi
{
    class CORE_API base64
    {
    public:
        static string encode(byte const* buf, uint bufLen);
        static vector<byte> decode(string const&);
    };
}