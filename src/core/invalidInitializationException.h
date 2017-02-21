#pragma once
#include <phi.h>
#include "coreApi.h"
#include "exception.h"

namespace phi
{
    class invalidInitializationException :
        public exception
    {
    public:
        CORE_API invalidInitializationException(string message);
    };
}