#ifndef _PHI_BASE64_H_
#define _PHI_BASE64_H_

#include "globals.h"

#include <vector>
#include <string>

namespace phi
{
    class base64
    {
    public:
        CORE_API static std::string encode(byte const* buf, unsigned int bufLen);
        CORE_API static std::vector<byte> decode(std::string const&);
    };
}

#endif