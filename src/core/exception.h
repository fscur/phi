#pragma once
#include "core.h"
#include <phi.h>

namespace phi
{
    class exception :
        public std::runtime_error
    {
    public:
        CORE_API exception(string message);
    };
}