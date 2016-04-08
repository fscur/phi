#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
    class CORE_API base64
    {
    public:
        static string encode(byte const* buf, uint bufLen);
        static vector<byte> decode(string const&);
    };
}